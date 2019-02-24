# Gradients & Color Bin

- 2x2 grid colors
- 4x4 grid colors



1. Dissect image into $3M\times3N. M,N \in \Z_+$

2. Calculate score against stored gradient and color map.

$$
k, l \text{ are contants.}\\
I \text{ is current image.}\\
S_i := k||\nabla I-G_i|| + l||\text{Bin}(\text{LowPass}(I))-\text{CBin}_i||
$$

3. Use image of max $S_i​$

