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

	return 0;
}