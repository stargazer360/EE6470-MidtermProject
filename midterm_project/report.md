# Midterm Project Report by 108061109 Chou Hsing Hang
In this project, a synthesizable module for k-ary searching on finding tangent of a point p and a convex polygon(half part) is implemented.
## Repositories
https://github.com/stargazer360/108061109-EE6470-HW3.git
## Implementation
There are four different parts of the tangent finder module: 
* get points p and k cuts points on the polygon
* checking whether tagent point is to left of, to right of or equal to each cut point
* deteremine which region the tangent point is in and the corresponding indices of left bound and right bound 
* output the indices

Base on the model, there are three different version of the module for synthesis as following:
* Loop Unrolling: completely unroll the loop in second and third part of the module
* Loop Unrolling with pipelined input: completely unroll the loop in second and third part and pipeline the first part of the module

For these design, the analysis of following part is based on testing each of them with k = 2, 4, 8 ,16 and different constraint of upper bound of cycle in second part of the module by their latency and area.
## Experimental result
The total theoical latency of finding tagent point with k-ary search is proper to $latency_{iteration}/log(k)$, denoting by $l_{eff}$ in following, and it is used as and index for comparing different simulation results.

The primitive design with k = 2 has latency = 19(cycle), area = 8238.1 and $l_{eff}$= 63.117.

Following table shows the simulation result. 
"-" indicate that the syntheis result is identical to the lower block. The unit of latency is cycle, which is 10ns in this project.

Loop Unrolling
| k     | 2       |         | 4       |         | 8       |         | 16      |         |
|-------|---------|---------|---------|---------|---------|---------|---------|---------|
| cycle | Latency | Area    | latency | Area    | Latency | Area    | Latency | Area    |
| 5     | -       | -       | -       | -       | 55      | 20672   | 111     | 35980.1 |
| 4     | -       | -       | 25      | 12820.7 | 50      | 20519   | 100     | 35689.6 |
| 3     | -       | -       | -       | -       | 39      | 22862.1 | 80      | 36795   |
| 2     | -       | -       | 17      | 14586.2 | 35      | 21742   | 71      | 36265   |
| 1     | 8       | 10347.5 | 16      | 16435.6 | 32      | 25384.9 | 64      | 39555.9 |
| 0     | 7       | 10192.7 | 13      | 18292.3 | 25      | 26070.7 | 49      | 45658.4 |

Loop Unrolling with pipelined input
| k     | 2       |         | 4       |         | 8       |         | 16      |         |
|-------|---------|---------|---------|---------|---------|---------|---------|---------|
| cycle | Latency | Area    | latency | Area    | Latency | Area    | Latency | Area    |
| 5     | -       | -       | 24      | 17303.7 | 50      | 28168.7 | 98      | 40820.5 |
| 4     | -       | -       | -       | -       | 45      | 28024.4 | 87      | 49574.9 |
| 3     | -       | -       | 16      | 19041   | 34      | 30530.3 | 67      | 50833.3 |
| 2     | 9       | 13240.4 |         |         | 30      | 29287.7 | 58      | 50326.5 |
| 1     | 8       | 14211.7 | 14      | 22498.9 | 26      | 32491.6 | 50      | 55501.9 |
| 0     | 7       | 16195.3 | 11      | 20501.6 | 19      | 37394.8 | 35      | 58326.4 |

Amoung these design, loop unrolling with pipelined input with k = 4 and 0 cycle upperbound in the block has the best $l_{eff}$=18.2706, about 28.9% of the $l_{eff} in the primitive design and increasing the area by $148.8%$. Applying DPA on this design further reduce the latency to 10 and $l_{eff}$ to 16.6, about 26% of the primitive design.

By taking the latency, 100ns, back to the TLM model, we have total simulation = 532ns with a convex polygon(half) with 256 points.

Noticing that in some of the condition, loosing the upper bound of the cycle constraint makes a worser result in area, which shows that the alogorithm of the HLS tool does not confirm to have the best synthesis result in area for the highest upper bound amoung others.

## Conclusion
Unrolling the calculateion part of the design help reduce the latency, but it takes relatively large amount of area to store and access those input and by using pipelined input, the constrained by constrained by the required of getting all of the input into the memory first in unrolling design can be reduced.