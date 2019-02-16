/* Spline Class made by Hubert Szymanski
 * Built on: Ubuntu 18.04
 * TODO:
 * - export plot to PNG file (maybe PDF?)
 * - try to save without turning on window (cos why should we really do that)
 * - get rid of GSL library (it's cool, but...)
*/

#ifndef SPLINE_H
#define SPLINE_H

#include <QApplication>
#include "mainwindow.h"
#include <QApplication>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

class Spline {

	public:
		// CONSTRUCTORS
        Spline();
		~Spline();

		// METHODS & FUNCTIONS
        // INTERFACE
        void calculateQuadraticSpline(double z0);
        void calculateCubicSpline(double z0, double zn);
		void printParameters();
		void saveToFile(std::string outFile, double dx);
        void drawPlot(bool bPlot, bool bPNG);
        void setInFile(std::string in);
        void setXRange(double xp, double xk);
        void setYRange(double yp, double yk);
        void setXLabel(std::string xlabel);
        void setYLabel(std::string ylabel);
		void clear();

	private:
		// VARIABLES
        bool readOK;
        bool quadraticSpline;
        bool cubicSpline;
        std::string inFile;
			// parameters
        QVector<double> vA;
        QVector<double> vB;
        QVector<double> vC;
        QVector<double> vD;
            // points
        QVector<double> vX;
        QVector<double> vY;
            // app stuff
        double x0, xn;  // X RANGE
        double y0, yn;  // Y RANGE
        std::string xname, yname; // LABELS NAMES

		// METHODS & FUNCTIONS
		void readPoints();
		void printPoints();
		void clearPoints();
            // for quadratic spline
        void createMatrix2( gsl_matrix *A, unsigned long int N );
        void createVectorZ2( gsl_vector *z, unsigned long int N, double &z0 );
        double getB2( int i );
        double getC2( int i );
            // for cubic spline
        void createMatrix3(unsigned long n, gsl_matrix *A);
        void createVectorZ3(unsigned long n, gsl_vector *z, double &z0, double &zn);
        void getParameters3( int i, gsl_vector *c );

};

#endif // SPLINE_H
