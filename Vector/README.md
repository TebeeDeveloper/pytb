# VectorLib
### 5WH
What? - This is a lib that you want to use if you (or your mind) see the 'numpy' so slow for your work (or not);

Why? - The Vector I write with C++ (or not) is the reallable to use for your work (or not);

Who? - You (or your mum);

When? - Anytime when you need to process an Array but you dont want to wait the 'numpy' importing;

Where? - Inlining your Project;

How? - 'pip install' this repo and you will have this vector anywhere you want (like your mum's house);

#### Disclaimer
- If you broke your RAM (not me), it's not my fault (yep). It's your computers (and your mum's computers);

- Dont ask me why it been. Ask your mum why she gave you a broom instead of a soldering iron;

- Dont ask me why I use primitive type instead Generic or void*. Ask why your computer (not my computer) was fired;

- Code was wrote by a student 15 years-old (yep, me) not like to use `std::vector` and hate the delays. If it faster than your code, dont cry;


### Performance
- Benchmark: It fast (faster than your mum broom);
- Memory: Minimal (fits your mum's Nokia 1100);

### Benchmark table
| Stage             | NumPy (s)  | VectorInt (s) |
|-------------------|------------|---------------|
| Import            | 0.601076   | 0.003640      |
| Load data         | 0.162039   | 0.024884      |
| Execute (sum)     | 0.005819   | 0.001490      |
| Execute (sort)    | 0.237492   | 0.055413      |
| Execute (find)    | 0.002529   | 0.000034      |
| Execute (dot)     | 0.002813   | 0.002763      |
| **TOTAL**         | **1.011769** | **0.088225**  |

### Usage Example
```C
#include "vector_int.h"
int main() {
    // Not to import numpy;
    // Not to wait;
    // Need the logical and speed;
    VectorInt *v = vector_int_init(20000000000);
    // It's over before you even blink;
    return 0;
}
```
