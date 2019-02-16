#include "spline.h"

//-----------------------------------------------
//	CONSTRUCTORS
//-----------------------------------------------

Spline::Spline() {
    // nooope
    readOK = false;
    quadraticSpline = false;
    cubicSpline = false;
    // default Xrange
    x0 = -1.0;
    xn = 1.0;
    // default Yrange
    y0 = -1.0;
    yn = 1.0;
    // default axes labels
    xname = "";
    yname = "";
}

Spline::~Spline() {
	clear();
}

//-----------------------------------------------
//	METHODS & FUNCTIONS
//-----------------------------------------------

void Spline::readPoints() {
	
	double x, y;
    std::ifstream input;
    input.open(inFile);
    if( !input.fail()  ){  // && input.tellg() != 0 to slabo sprawdza czy jest pusty :(
        while ( input >> x >> y ) {
            // check if x are sorted (IMPORTANT !!!)
            if ( vX.empty() || x > vX.back() ) {
                vX.push_back( x );
                vY.push_back( y );
                readOK = true;
            }
            else {
            // DODAĆ WYRZUCANIE BLEDU !!!
                std::cerr << "ERROR: wrong point: " << x << ", " << y << std::endl;
                std::cerr << "\tPlease provide sorted data\n";
            }
        }
    }
    else {
        readOK = false;
    }
}

void Spline::printPoints() {
	std::cout << "\nPOINTS FROM INPUT" << std::endl;
	std::cout << "x\ty" << std::endl;
	std::cout << "---------------------" << std::endl;
    for ( auto i = 0; i < vX.size(); i++ ) {
		std::cout << vX[i] << "\t" << vY[i] << std::endl;
	}
	std::cout << "---------------------" << std::endl;
}

void Spline::clearPoints() {
	
	vX.clear();
	vY.clear();
}

void Spline::calculateQuadraticSpline(double z0) {

	// read points
	readPoints();

    if ( !readOK ) {
        // THROW THAT NOPE !!! C'MON
        std::string nope {"ERROR: Input file read failed"};
        throw nope;
    }

	// print them
//	printPoints();
    // well... more than about 25 pts it would be breathtakin' for terminal ]:>
    // anyway who needs them if they are in input file

	// size of square Matrix and Vectors for Ab = z
    auto N = static_cast<unsigned long>( vX.size() );
	// create matrix A
	gsl_matrix *A;
	A = gsl_matrix_calloc( N, N );
	createMatrix2( A , N );
//	gsl_matrix_fprintf( stdout, A, "%f" );

	// create vectors b and z
	gsl_vector *b, *z;
	b = gsl_vector_alloc( N );
	z = gsl_vector_alloc( N );
    createVectorZ2( z, N, z0 );

	// solve Ab = z
	gsl_permutation *p;
    p = gsl_permutation_alloc( N );
	int s;
	gsl_linalg_LU_decomp(A, p, &s);
	gsl_linalg_LU_solve(A, p, z, b);

    vA.clear(); // just in case, for safety
    for ( auto i = 1; i < static_cast<int>(N); i++ ) {
		vA.push_back( vY[i-1] );
	}

	// move to vB
    vB.clear(); // just in case, for safety
    for ( auto i = 1; i < static_cast<int>(N); i++ ) {
        vB.push_back( gsl_vector_get( b, static_cast<unsigned long>(i-1) ) );
	}
	
    vC.clear(); // just in case, for safety
    for ( auto i = 1; i < static_cast<int>(N); i++ ) {
        auto dz = gsl_vector_get( b, static_cast<unsigned long>(i) ) - gsl_vector_get( b, static_cast<unsigned long>(i-1) );
		vC.push_back( dz / ( 2.0*( vX[i] - vX[i-1] ) ) );
	}
	
	gsl_matrix_free( A );
	gsl_vector_free( b );
	gsl_vector_free( z );
	gsl_permutation_free( p );

    // set booleans
    quadraticSpline = true;
    cubicSpline = false;
}

void Spline::calculateCubicSpline(double z0, double zn) {

    // read points
    readPoints();
    // check if read was ok
    if ( !readOK ) {
        // THROW THAT NOPE !!! C'MON
        std::string nope {"ERROR: Input file read failed"};
        throw nope;
    }
    // check if data is valid
    if ( vX.size() != vY.size() ) {
        std::string ouch {"ERROR: Points missing arguments"};
        throw ouch;
    }

    if ( vX.empty() ) {
        std::string whatNow {"ERROR: no POINTS declared"};
        throw whatNow;
    }

    auto n = static_cast<unsigned long>( vX.size() );
    // create matrix A
    gsl_matrix *A;
    A = gsl_matrix_alloc(n, n);
    createMatrix3(n, A);
    // create vector z
    gsl_vector *z;
    z = gsl_vector_alloc(n);
    createVectorZ3(n, z, z0, zn);

    // create vector c
    gsl_vector *c;
    c = gsl_vector_alloc(n);

    // solve A*c = z
    gsl_permutation *p;
    p = gsl_permutation_alloc( n );
    int s;
    gsl_linalg_LU_decomp(A, p, &s);
    gsl_linalg_LU_solve(A, p, z, c);

    // clear parameters vectors
    vA.clear();
    vB.clear();
    vC.clear();
    vD.clear();
    for( auto i = 0; i < static_cast<int>(n-1); i++ ) {

        getParameters3( i, c );
    }

    // clean that mess
    gsl_matrix_free( A );
    gsl_vector_free( c );
    gsl_vector_free( z );
    gsl_permutation_free( p );

    // set booleans
    quadraticSpline = false;
    cubicSpline = true;
}

void Spline::printParameters() {
    // Gimme calculated parameters <3
    // show me what we got !!!
	std::cout << "\nCALCULATED PARAMETERS\n";
    std::cout << std::setprecision(6) << std::fixed;

    // parameters for quadratic Spline
    if( quadraticSpline ) {
        std::cout << "a\tb\tc" << std::endl;
        std::cout << "---------------------\n";
        // maybe someday, maybe someway
        // precision change ?! printParameters(int prec) <<< THAT'S THE DREAM !!!

        for ( auto i = 0; i < vA.size(); i++ ) {
            std::cout << vA[i] << "\t" << vB[i] << "\t" << vC[i] << std::endl;
        }
    }

    // parameters for cubic Spline
    else if( cubicSpline ) {
        std::cout << "a\tb\tc\td" << std::endl;
        std::cout << "---------------------\n";
        for ( auto i = 0; i < vA.size(); i++ ) {
            std::cout << vA[i] << "\t" << vB[i] << "\t" << vC[i] << "\t" << vD[i] << std::endl;
        }
    }

    // "print-block" ending line to make it look soo beauty <3
	std::cout << "---------------------\n";
}

void Spline::saveToFile(std::string outFile, double dx) {
	// open outfile
	std::ofstream out;
	out.open( outFile );
    // check if file is opened nicely or ask LEO WHY ?
    if( out.fail() ){
        std::string leoWhy {"ERROR: Out file opening failed!!!"};
        throw leoWhy;
        // leoWhy = "FOR MONEYYY !!!";
        // I KNOW IT WON'T BE EXECUTED...
        // I just had to... OK ?!
    }
	// define variables
    auto i = 0;
	double x, y, h;
    // woahh... PRECISION ?! AGAIN ?! ...
    // fine... I'll let U stay there... or ?
    out << std::setprecision(6) << std::fixed;
	// write points to file
	out << "X\tY\n";

    if( quadraticSpline ) {
        for( x = vX[0]; x <= vX.back(); x += dx ) {
            if ( x > vX[i+1] ) i++;
            h = x - vX[i];
            y = vA[i] + vB[i]*h + vC[i]*h*h;
            out << x << "\t" << y << "\t" << i << std::endl;
        }
        // to draw till last point
        x = vX.back();
        h = x - vX[i];
        y = vA[i] + vB[i]*h + vC[i]*h*h;
        out << x << "\t" << y << "\t" << i << std::endl;
    }

    else if( cubicSpline ) {
        for( x = vX[0]; x <= vX.back(); x += dx ) {
            if ( x > vX[i+1] ) i++;
            h = x - vX[i];
            y = vA[i] + vB[i]*h + vC[i]*h*h;
            out << x << "\t" << y << "\t" << i << std::endl;
        }
        // to draw till last point
        x = vX.back();
        h = x - vX[i];
        y = vA[i] + vB[i]*h + vC[i]*h*h + vD[i]*h*h*h;
        out << x << "\t" << y << "\t" << i << std::endl;
    }

	// close file
	out.close();
}

void Spline::setInFile(std::string inF) {
    if ( inF == "" ) {
        std::string err = "ERROR: input filename empty!";
        throw err;
    }
    inFile = inF;
}

void Spline::drawPlot(bool bPlot, bool bPNG) {

    // create application
    int argc;
    char** argv;
    QApplication a(argc, argv);
    // create Window object
    MainWindow w;
    // show it
    w.show();

    // set axes
    //*/ blok auto
    x0 = *std::min_element(vX.begin(), vX.end()) -1.0;
    xn = *std::max_element(vX.begin(), vX.end()) +1.0;
    y0 = *std::min_element(vY.begin(), vY.end()) -1.0;
    yn = *std::max_element(vY.begin(), vY.end()) +1.0;
    //*/
    w.setXAxis(x0, xn, xname);
    w.setYAxis(y0, yn, yname);

    // define variables
    auto i = 0;
    double x, y, h;
    QVector<double> pX, pY;
    double dx {0.05};

    // prepare data to plot ->  this should be like other function maybe...
    if( quadraticSpline ) {
        for( x = vX[0]; x <= vX.back(); x += dx ) {
            if ( x > vX[i+1] ) i++;
            h = x - vX[i];
            y = vA[i] + vB[i]*h + vC[i]*h*h;
            pX.push_back(x);
            pY.push_back(y);
        }
        // to draw till last point
        pX.push_back( vX.back() );
        pY.push_back( vY.back() );
    }

    else if( cubicSpline ) {
        for( x = vX[0]; x <= vX.back(); x += dx ) {
            if ( x > vX[i+1] ) i++;
            h = x - vX[i];
            y = vA[i] + vB[i]*h + vC[i]*h*h + vD[i]*h*h*h;
            pX.push_back(x);
            pY.push_back(y);
        }
        // to draw till last point
        pX.push_back( vX.back() );
        pY.push_back( vY.back() );
    }

    // plot data
    w.plot(pX, pY, vX, vY);

    if ( bPNG )
        w.savePNG();

    // execute aplication
    // runs a app loop
    if ( bPlot ) {
        int check = a.exec();
        // check it... dunno why, but let's do this
        if( check != 0 ) std::cout << "Sth went wrong with ploting ?!\n";
    }
}

// ahh... that "useless" methods <3
void Spline::setXRange(double xp, double xk){
    x0 = xp;
    xn = xk;
}

void Spline::setYRange(double yp, double yk){
    y0 = yp;
    yn = yk;
}

void Spline::setXLabel(std::string xlabel){
    xname = xlabel;
}

void Spline::setYLabel(std::string ylabel){
    yname = ylabel;
}
// just kiddin... clear interface for main() is important
void Spline::clear() {
	clearPoints();
	vA.clear();
	vB.clear();
	vC.clear();
    vD.clear();
    quadraticSpline = false;
    cubicSpline = false;
}

void Spline::createMatrix2( gsl_matrix *A , unsigned long int N ) {
	gsl_matrix_set( A, 0, 0, 1.0 );
    for ( unsigned long int i = 1; i < N; i++) {
		gsl_matrix_set( A, i, i-1, 1.0 );
		gsl_matrix_set( A, i, i , 1.0 );
	}
}

void Spline::createVectorZ2( gsl_vector *z, unsigned long int N, double &z0 ) {
    gsl_vector_set( z, 0, z0 );
    for ( auto i = 1; i < static_cast<int>(N); i++ ) gsl_vector_set( z, static_cast<unsigned long>(i), 2.0*( vY[i] - vY[i-1] ) / ( vX[i] - vX[i-1] ) );
}

double Spline::getB2( int i ) {
	double b, hx;
	// step between points
	hx = vX[i+1] - vX[i];
	// b parameter
	b = 2.0*( vY[i+1] - vY[i] ) / hx  - vB[i];

	return b;
}

double Spline::getC2( int i ) {
	double c, hx;
	// step between points
	hx = vX[i+1] - vX[i];
	// b parameter
	c = ( vB[i+1] - vB[i] ) / ( 2.0*hx );

	return c;
}

void Spline::createMatrix3( unsigned long n, gsl_matrix *A) {
    gsl_matrix_set( A, 0, 0, vX[1] - vX[0]);
    for( auto i = 0; i < static_cast<int>(n); i++ )
        for ( auto j = i; j < static_cast<int>(n); j++) {
            // diagonal
            if ( j == i ) {
                if ( i == 0 ) gsl_matrix_set( A, static_cast<unsigned long>(i), static_cast<unsigned long>(i), 2.0*( vX[1] - vX[0] ) );
                else if ( i == static_cast<int>(n - 1)) gsl_matrix_set( A, static_cast<unsigned long>(i), static_cast<unsigned long>(i), 2.0*( vX[static_cast<int>(n-1)] - vX[static_cast<int>(n-2)] ) );
                else gsl_matrix_set( A, static_cast<unsigned long>(i), static_cast<unsigned long>(i), 2.0*( vX[i+1] - vX[i-1] ) );
            }
            // smaller diagonals
            else if (j - i == 1) {
                gsl_matrix_set( A, static_cast<unsigned long>(i), static_cast<unsigned long>(j), vX[i+1] - vX[i] );
                gsl_matrix_set( A, static_cast<unsigned long>(j), static_cast<unsigned long>(i), gsl_matrix_get( A, static_cast<unsigned long>(i), static_cast<unsigned long>(j) ) );
            }
            // zeros :(
            else {
                gsl_matrix_set( A, static_cast<unsigned long>(i), static_cast<unsigned long>(j), 0.0);
                gsl_matrix_set( A, static_cast<unsigned long>(j), static_cast<unsigned long>(i), 0.0);
            }
        }
/*    std::cout << "Matrix A: test:\n";
    for( auto i = 0; i < static_cast<int>(n); i++ ) {
        for( auto j = 0; j < static_cast<int>(n); j++ )
            std::cout << gsl_matrix_get( A, static_cast<unsigned long>(i), static_cast<unsigned long>(j) );
        std::cout << std::endl;
    }
    std::cout << "--------------------\n";
*/
}

void Spline::createVectorZ3(unsigned long n, gsl_vector *z, double &z0, double &zn) {

    // first element, where z0 means f'(x0)
    gsl_vector_set( z, 0, 3.0*( ( vY[1] - vY[0] ) / ( vX[1] - vX[0] ) - z0 ) );

    // middle emenets
    for( auto i = 1; i < static_cast<int>(n - 1); i++) {
        // just to make things clear
        double dyi = vY[i+1] - vY[i];
        double dyi_1 = vY[i] - vY[i-1];
        double hi = vX[i+1] - vX[i];
        double hi_1 = vX[i] - vX[i-1];
        gsl_vector_set( z, static_cast<unsigned long>(i), 3.0*( dyi/hi - dyi_1/hi_1 ) );
    }
    // last element, where zn means f'(xn)
    gsl_vector_set( z, n-1, 3.0*( zn - (vY[static_cast<int>(n-1)] - vY[static_cast<int>(n-2)])/(vX[static_cast<int>(n-1)] - vX[static_cast<int>(n-2)]) ) );

}

void Spline::getParameters3( int i, gsl_vector *c ) {
    // create helpful variables
    double A, B, C, D;

    // create more helpful variables to make everything more readable
    double ci = gsl_vector_get( c, static_cast<unsigned long>(i) );
    double ci1 = gsl_vector_get( c, static_cast<unsigned long>(i+1) );
    double hi = vX[i+1] - vX[i];

    // calculate parameters
    A = vY[i];
    B = ( vY[i+1] - vY[i] ) / hi - ( 2.0*ci*hi + ci1*hi ) / 3.0;
    C = ci;
    D = ( ci1 - ci ) / ( 3.0*hi );

    // push them back
    vA.push_back( A );
    vB.push_back( B );
    vC.push_back( C );
    vD.push_back( D );
}
