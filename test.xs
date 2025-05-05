var star=0;
star=star+1;
var space=0;
var count=0;
	var BAILOUT = 16;
	var MAX_ITERATIONS = 1000;
function mandelbrot(var x, var y) {
	var cr = y - 0.5;
	var ci = x;
	var zi = 0.0;
	var zr = 0.0;
	var i = 0;

	while(1) {
		i=i+1;
		count=count+1;
		var temp = zr * zi;
		var zr2 = zr * zr;
		var zi2 = zi * zi;
		zr = zr2 - zi2 + cr;
		zi = temp + temp + ci;
		if (zi2 + zr2 > BAILOUT) {
			return i;
		}
		if (i > MAX_ITERATIONS) {
			return 0;
		}
	}
}

function mandelbrot_run() {
	var x; var y;
	
	for (y = -39; y < 39; y=y+1) {
		io_print("\n");
		for (x = -39; x < 39; x=x+1) {
			var i = mandelbrot(x/40.0, y/40.0);
			if (i==0) {
				io_print("*");
				star=star+1;
			}
			else {
				io_print(" ");
				space=space+1;
			}
		}	
	}
}

mandelbrot_run();
io_print("\ncount:"..count);
io_print("\nmandelbrot_run success done! from xs script\n");
