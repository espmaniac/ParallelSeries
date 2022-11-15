#include <iostream>
#include "parallel_series.hpp"


int main() {
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

	double r = resistor.solve();
	printf("resistor = %f\n", r);

	Star resistorStar = resistor.deltaStarTransform({.a=12.0, .b=18.0, .c=6.0});

	printf("resistor Delta(%f, %f, %f) =\n Star(%f, %f, %f) = c + (a || b) = %f\n", 
		12.0, 18.0, 6.0, 
		resistorStar.a, resistorStar.b, resistorStar.c,
		resistor.onSeries(resistorStar.c, resistor.onParallel(resistorStar.a, resistorStar.b))
	);

	resistor.reset();
	resistor.expr = "2k + 2K || 2kilo"; // prefixes -> 2000 + 2000 || 2000
	printf("prefixes = %.20lf\n", resistor.solve()); // result 3000

	ParallelSeries capacitor;

	capacitor.onSeries = [](double left, double right) -> double {
		return (left != 0 && right != 0) ? (1 / (1/left + 1/right)) : 0;
	};

	capacitor.onParallel = [](double left, double right) -> double {
		return left + right;
	};

	// result 1.3333
	capacitor.expr = "2 + 2 // 2";

	double c = capacitor.solve();

	printf("capacitor = %f\n", c);

	Star capacitorStar = capacitor.deltaStarTransform({.a=12.0, .b=18.0, .c=6.0});

	printf("capacitor Delta(%f, %f, %f) =\n Star(%f, %f, %f) = c + (a || b) = %f\n", 
		12.0, 18.0, 6.0, 
		capacitorStar.a, capacitorStar.b, capacitorStar.c,
		capacitor.onSeries(capacitorStar.c, capacitor.onParallel(capacitorStar.a, capacitorStar.b))
	);

	return 0;
}