/* Program by Hubert Szymanski
 * Built on: Ubuntu 18.04
 * TODO:
 * - handle input arguments to argv (CLI input)
 * - create man / help
 *
 * CLI to handle
 * - get file input path !!!
 * - saveToFile & drawPlot FLAGS
 * - printParameters FLAGS
 * - savePNG FLAG
 * - set axes parameters (?!) -> auto flag :D
 * ?
*/

#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "spline.h"

using namespace std;

void printHelp();

int main(int argc, char *argv[])
{
    // check flags from CLI
    if ( argc < 2 ) {
        cout << "NO PARAMETERS TO RUN PROGRAM \n\n";
        printHelp();
        return -1;
    }
    // setup from command line
    string inFile;
    string outFile;
    // flags from input
    bool bPrintParameters = false;
    bool bSaveToFile = false;
    bool bDrawPlot = false;
    bool bSavePNG = false;
    bool bQuadratic = false;
    bool bCubic = true;


    for ( auto i = 0; i < argc; i++ ) {
        if ( argv[i][0] == '-' )
            switch ( argv[i][1] ){
            // define spline degree
            case '2':
                bQuadratic = true;
                bCubic = false;
                break;
            case '3':
                bCubic = true;
                bQuadratic = false;
                break;
            // set input file
            case 'i':
                if (argc < i+2) {cerr << "ERROR: empty filename\n"; return -1;}
                inFile.append(argv[++i]);
                break;
            // save to file and set outFile
            case 's':
                bSaveToFile = true;
                if (argc < i+2) {cerr << "ERROR: empty filename\n"; return -1;}
                outFile.append(argv[++i]);
                break;
            // print parameters / draw a plot / export PNG
            case 'p':
                bPrintParameters = true;
                break;
            case 'd':
                bDrawPlot = true;
                break;
            case 'e':
                bSavePNG = true;
                break;
            // print help and quit
            case 'h':
                printHelp();
                return 0;
            // default: unknown flag
            default:
                cerr << "WARNING: unknown flag parameter !!!\n";
                printHelp();
                break;
            }
    }

    // check if ONE spline degree is set
    if ( (!bQuadratic && !bCubic) || (bQuadratic && bCubic) ) {
        cerr << "ERROR: chose Quadratic or Cubic spline\n";
        return -1;
    }

    // create spl object and try to do everything
    Spline spl;
    try {
        spl.setInFile(inFile);
        if ( bQuadratic )
            spl.calculateQuadraticSpline(10.0);
        if ( bCubic )
            spl.calculateCubicSpline(0.0, 0.0);
        if ( bPrintParameters )
            spl.printParameters();
        if ( bDrawPlot || bSavePNG ) {
            spl.setXRange(4.0, 10.0);
            spl.setYRange(-1.0, 6.0);
            spl.setXLabel("x");
            spl.setYLabel("f(x)");
            spl.drawPlot(bDrawPlot, bSavePNG);
        }
        if ( bSaveToFile )
            spl.saveToFile(outFile, 0.1);
    } // end of try
    catch (std::string err) {
        cout << err << endl;
        printHelp();
    } // end of catch

    return 0;
}

void printHelp() {
    // this function is used to print manual / help
    // to use this program

    cout << "--- Spline Project Help ---\n";
    cout << "USAGE:\n./SplineProject [flags]\n";
    cout << "\nTo choose Spline degree\n"
            " -2\t\tQuadratic spline\n"
            " -3\t\tCubic spline\n";
    cout << "\nSetting input / output\n"
            " -i [filePath]\tSet input file with points to interpolate\n"
            " -s [fileName]\tSet output file to write interpolated function\n";
    cout << "\nOther flags\n"
            " -d\t\tDraws the spline function from read points\n"
            " -e\t\tExports plot to PNG file named outplot\n"
            " -p\t\tPrint parameters of polynominal used to spline given points\n"
            " -h\t\tShows this help info\n";
    cout << "\nProgram NEEDS to get at least Spline degree flag,\n"
            "input file with points to interpolate,\n"
            "and to see any output any other flag (to draw, save etc.)\n";
}
