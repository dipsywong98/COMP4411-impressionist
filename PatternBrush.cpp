#include "PatternBrush.h"
#include <filesystem>
#include <FL/fl_ask.H>
#include "CurvedBrush.h"
#include <FL/Fl_Native_File_Chooser.H>

std::vector< ImageWrapper<GLubyte> > PatternBrush::patterns{};
std::vector< ImageWrapper<GLubyte> > PatternBrush::scaledPatterns{};
std::random_device PatternBrush::rd{};
std::default_random_engine PatternBrush::gen{rd()};
std::uniform_int_distribution<> PatternBrush::dis(1,10);
int PatternBrush::size = -1;

void PatternBrush::cb_set_patterns(Fl_Widget*, void*)
{
	Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	chooser.directory(std::filesystem::current_path().string().c_str());
	chooser.show();

	auto* directoryName = chooser.filename();

	if (directoryName == nullptr)
	{
		fl_alert("No directory selected!");
	}
	else
	{
		setPatterns(directoryName);
	}
}


void PatternBrush::setPatterns(const std::string& directoryName)
{
	const std::filesystem::path patternDirectory(directoryName);

	if (is_directory(patternDirectory))
	{
		patterns.clear();

		for (auto& entry: std::filesystem::directory_iterator(patternDirectory))
		{
			if (entry.path().extension() == ".bmp")
			{
				//Entry should be an alpha map.
				auto width = 0;
				auto height = 0;

				patterns.push_back({
					readBMP(entry.path().string().c_str(), width, height),
					{width, height}
				});
			}
		}

		dis = std::uniform_int_distribution<>(0, patterns.size() - 1);
	}
	else
	{
		fl_alert("Selected path is not a directory.");
	}
}

PatternBrush::PatternBrush(ImpressionistDoc* pDoc, char*): ImpBrush(pDoc)
{
}

void PatternBrush::BrushBegin(const Point source, const Point target)
{
	const auto newSize = GetDocument()->m_pUI->getSize();

	if (newSize != size)
	{
		size = newSize;
		//make scaled images
		const Dim newDim = { size, size };
		scaledPatterns.clear();
		for (auto& w: patterns)
		{
			ImageWrapper<GLubyte> v{
				new GLubyte[newDim.getLength()],
				newDim
			};

			const auto scale = double(w.dim.width) / size;

			v.eachPixel([&](GLubyte* rgbArray, const long x, const long y)
			{
				auto* t = w.getPixelPtr(x * scale, y * scale);

				rgbArray[0] = t[0];
				rgbArray[1] = t[1];
				rgbArray[2] = t[2];
			});

			scaledPatterns.push_back(v);
		}
	}

	BrushMove(source, target);
}

void PatternBrush::BrushMove(const Point source, const Point target)
{
	auto* docPtr = GetDocument();

	if (docPtr == nullptr) {
		__debugbreak();
		return;
	}

	if (source.x > docPtr->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	if (step == 0)
	{
		//Make new pattern
		auto& alpha = scaledPatterns[dis(gen)];

		const int tX0 = target.x - alpha.dim.width / 2;
		const int tY0 = target.y - alpha.dim.height / 2;

		glPointSize(1.f);
		glBegin(GL_POINTS);
		{
			alpha.eachPixel([&](const GLubyte* rgbArray, const long x, const long y)
			{
				const auto tX = tX0 + x;
				const auto tY = tY0 + y;

				if (tX < 0 || tX >= docPtr->m_nPaintWidth || tY < 0 || tY >= docPtr->m_nPaintHeight) return;

				if (rgbArray[0] > 0)
				{
					docPtr->m_pUI->setAlpha(rgbArray[0] / 255.0);
					SetColor(source);
					glVertex2d(tX, tY);
				}
			});
		}
		glEnd();

		step = alpha.dim.width / 4;
	}
	else
	{
		step--;
	}
}

void PatternBrush::BrushEnd(const Point source, const Point target)
{
}
