Calculator for calculating series or parallel or delta or mixed connections of resistors, capacitors, inductance coils.

# Code Examples

## Resistor Or Inductor
```c++
ParallelSeries resistor;

resistor.onSeries = [](double left, double right) -> double {
  return left + right;
};
resistor.onParallel = [](double left, double right) -> double {
  return (left != 0 && right != 0) ? (1 / (1/left + 1/right)) : 0;
};

// result 3
resistor.expr = "2 + 2 || 2"; // same as 2 + (2 || 2) 

/*"//" == "||" */
//resistor.expr = "2 // (2 + 2)"; // result 1.333

//resistor.expr = "((3 + (5 + 6)) // 4) + 1"; // 4.111111 

double r = resistor.solve();
printf("\nresistor = %f\n", r);
```
## Capacitor
```c++
ParallelSeries capacitor;

capacitor.onSeries = [](double left, double right) -> double {
  return (left != 0 && right != 0) ? (1 / (1/left + 1/right)) : ((left > right) ? left : right);
};

capacitor.onParallel = [](double left, double right) -> double {
  return (left != 0 && right != 0) ? (left + right) : 0;
};

// result 1.3333
capacitor.expr = "2 + 2 // 2";

double c = capacitor.solve();

printf("\ncapacitor = %f\n", c);
```

## Prefixes Support
```c++
something.expr = "2k + 2k || 2kilo"; // prefixes -> 2000 + 2000 || 2000
```
