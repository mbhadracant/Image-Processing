#include <wx/wx.h>
#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <math.h>
#include <map>
#include <stack>
#include <window.h>

using namespace std;

static const wxChar* FILETYPES = _T("All files (*.*)|*.*");
static const wxChar* RAW_FILETYPE = _T("RAW files (*.raw)|*.raw");

IMPLEMENT_APP(BasicApplication)

bool BasicApplication::OnInit()
{
    wxInitAllImageHandlers();
    MyFrame* frame = new MyFrame(_("Image Processing"), 50, 50, 800, 600);
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
}

MyFrame::MyFrame(const wxString title, int xpos, int ypos, int width, int height)
    : wxFrame((wxFrame*)NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height))
{

    canSelect = false;

    menuBar = new wxMenuBar;

    //###########################################################//
    //----------------------START MY MENU -----------------------//
    //###########################################################//
    fileMenu = new wxMenu;
    fileMenu->Append(LOAD_FILE_ID, _T("&Open file"));
    fileMenu->Append(OPEN_RAW_ID, _T("&Open RAW file")); //--->To be modified!
    fileMenu->Append(SAVE_RAW_TO_JPG, _T("&RAW Image to JPG"));

    fileMenu->Append(RESET_IMAGE_ID, _T("&Reset image"));
    fileMenu->Append(UNDO, _T("&Undo"));

    fileMenu->AppendSeparator();
    fileMenu->Append(SAVE_IMAGE_ID, _T("&Save image"));
    fileMenu->Append(EXIT_ID, _T("E&xit"));

    menuBar->Append(fileMenu, _T("&File"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    editMenu = new wxMenu;
    editMenu->Append(SELECT, _T("&Select"));
    editMenu->Append(UNSELECT, _T("&Unselect"));

    menuBar->Append(editMenu, _T("&Edit"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab4Menu = new wxMenu;
    lab4Menu->Append(SCALE_IMAGE_ID, _T("&Scale"));
    lab4Menu->Append(PIXEL_SHIFT_ID, _T("&Shift"));
    menuBar->Append(lab4Menu, _T("&Pixel"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab5Menu = new wxMenu;
    lab5Menu->Append(AVERAGE_FILTER_ID, _T("&Average Filter"));
    lab5Menu->Append(WEIGHTED_AVERAGE_FILTER_ID, _T("&Weighted Average Filter"));
    lab5Menu->Append(FOUR_NEIGHBOUR_FILTER_ID, _T("&4 Neighbour LapLacian Filter"));
    lab5Menu->Append(EIGHT_NEIGHBOUR_FILTER_ID, _T("&8 Neighbour LapLacian Filter"));
    lab5Menu->Append(FOUR_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID, _T("&4 Neighbour Laplacian Enhancement Filter"));
    lab5Menu->Append(EIGHT_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID, _T("&8 Neighbour Laplacian Enhancement Filter"));
    lab5Menu->Append(ROBERTS_ID, _T("&Roberts Filter"));
    lab5Menu->Append(SOBEL_X, _T("&Sobel X Filter"));
    lab5Menu->Append(SOBEL_Y, _T("&Sobel Y Filter"));

    menuBar->Append(lab5Menu, _T("&Image Convolution"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab6Menu = new wxMenu;

    lab6Menu->Append(SALT_AND_PEPPER_ID, _T("&Salt And Pepper Filter"));
    lab6Menu->Append(MIN_ID, _T("&Min Filter"));
    lab6Menu->Append(MAX_ID, _T("&Max Filter"));
    lab6Menu->Append(MIDPOINT_ID, _T("&Midpoint Filter"));
    lab6Menu->Append(MEDIAN_ID, _T("&Median Filter"));

    menuBar->Append(lab6Menu, _T("&Order-statistics Filtering"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab7Menu = new wxMenu;

    lab7Menu->Append(NEGATIVE_ID, _T("&Negative"));
    lab7Menu->Append(LOG_ID, _T("&Log"));
    lab7Menu->Append(POWER_ID, _T("&Power"));
    lab7Menu->Append(RANDOM_LOOKUP_TABLE, _T("&Random Lookup Table"));
    menuBar->Append(lab7Menu, _T("&Point Processing"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab8Menu = new wxMenu;
    lab8Menu->Append(HISTOGRAM_EQUALIZE, _T("&Equalize"));
    menuBar->Append(lab8Menu, _T("&Histogram"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab9Menu = new wxMenu;
    lab9Menu->Append(MEAN_AND_STANDARD_DEVIATION, _T("&Mean And Standard Deviation"));
    lab9Menu->Append(SIMPLE_THRESHOLDING, _T("&Simple Thresholding"));
    lab9Menu->Append(AUTOMATED_THRESHOLDING, _T("&Automated Thresholding"));
    menuBar->Append(lab9Menu, _T("&Thresholding"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    SetMenuBar(menuBar);
    CreateStatusBar(3);
    loadedImage = wxImage("../images/Lena.bmp");
    origImage = loadedImage.Copy();
    mouseDownX = 0;
    mouseDownY = 0;
    mouseUpX = loadedImage.GetWidth();
    mouseUpY = loadedImage.GetHeight();

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

MyFrame::~MyFrame()
{
    /* release resources */
    if (loadedImage.Ok()) {
        loadedImage.Destroy();
    }

    if (origImage.Ok()) {
        origImage.Destroy();
    }
}

void MyFrame::ResetDimensionsToOriginal() {
  if(selected) {
    selected = false;
  }
  mouseDownX = 0;
  mouseDownY = 0;
  mouseUpX = loadedImage.GetWidth();
  mouseUpY = loadedImage.GetHeight();
}

void MyFrame::UseOriginalImageIfNoTemp() {
  if (!tmpImage.Ok()) {
    tmpImage = loadedImage.Copy();
  }
}

void MyFrame::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{

    wxFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), FILETYPES, wxFD_OPEN, wxDefaultPosition);

    if (openFileDialog.ShowModal() == wxID_OK) {
        wxString filename = openFileDialog.GetFilename();
        wxString path = openFileDialog.GetPath();
        printf("Loading image form file...");

        if (loadedImage.Ok()) {
            loadedImage.Destroy();
        }

        if (origImage.Ok()) {
            origImage.Destroy();
        }

        loadedImage = wxImage(path); //Image Loaded form file

        if (loadedImage.Ok()) {
            origImage = loadedImage.Copy();
            ResetDimensionsToOriginal();
            printf("Done! \n");
        }
        else {
            printf("error:...");
        }
    }
    Refresh();
}

//###########################################################//
//-----------------------------------------------------------//
//---------- DO NOT MODIFY THE CODE ABOVE--------------------//
//-----------------------------------------------------------//
//###########################################################//

//INVERT IMAGE
void MyFrame::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    printf("Undoing...\n");
    if(undoStack.size() > 2) {
      undoStack.pop();
      loadedImage = undoStack.top();
      Refresh();
      origImageStack.pop();

      undoStack.pop();

  }
    printf("Finished Undoing.\n");
}

int round_int(double r)
{
    return (r > 0.0) ? (r + 0.5) : (r - 0.5);
}

//IMAGE SCALEING
void MyFrame::OnScaleImage(wxCommandEvent& WXUNUSED(event))
{

    printf("Scaling...\n");
    UseOriginalImageIfNoTemp();
    float scaleFact = 0;
    double input = 0;
    unsigned char r, g, b;
    wxString valueTyped;
    wxTextEntryDialog myDialog(this, _("Enter the pixel scale factor:"), _("Pixel Scaling"), _(""));

    if (myDialog.ShowModal() == wxID_OK) {
        valueTyped = myDialog.GetValue();
        if (valueTyped.ToDouble(&input)) {
            scaleFact = (float)input;
        }
        else {
            cout << "Invalid Input: " << valueTyped << endl;
            return;
        }
    }
    else {
        return;
    }



    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {
            r = tmpImage.GetRed(i, j); // red pixel value
            g = tmpImage.GetGreen(i, j); // green pixel value
            b = tmpImage.GetBlue(i, j); // blue pixel value

            //printf("(%d,%d) [r = %d  | g = %d | b = %d] \n",i,j,(unsigned)r,(unsigned)g,(unsigned)b);
            unsigned char newR, newG, newB;

            if ((r > 255 / scaleFact)) {
                newR = 255;
            }
            else {
                newR = r * scaleFact;
            }
            if ((g > 255 / scaleFact)) {
                newG = 255;
            }
            else {
                newG = g * scaleFact;
            }
            if ((b > 255 / scaleFact)) {
                newB = 255;
            }
            else {
                newB = b * scaleFact;
            }

            loadedImage.SetRGB(i, j, newR,
                newG,
                newB);
        }
    }
    ResetDimensionsToOriginal();
    tmpImage.Destroy();

    printf(" Finished scaling.\n");
    Refresh();
}



void MyFrame::OnOpenRawFile(wxCommandEvent& WXUNUSED(event))
{
    printf("Opening Raw Image...\n");

    wxFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), RAW_FILETYPE, wxFD_OPEN, wxDefaultPosition);

    if (openFileDialog.ShowModal() == wxID_OK) {
        wxString name = openFileDialog.GetFilename();
        wxString path = openFileDialog.GetPath();

        string filepath = "../images/";
        string filename = name.ToStdString();
        filepath += filename;

        if (loadedImage.Ok()) {
            loadedImage.Destroy();
        }

        if (origImage.Ok()) {
            origImage.Destroy();
        }

        if (filename == "Cameraman.raw") {
            loadedImage = wxImage(256, 256, true);
        }
        else {
            loadedImage = wxImage(512, 512, true);
        }

        if (loadedImage.Ok()) {
            origImage = loadedImage.Copy();
        }

        //load raw file
        int height = loadedImage.GetHeight();
        int width = loadedImage.GetWidth();
        unsigned char bufferImage[height][width];
        FILE* pFile;
        pFile = fopen(filepath.c_str(), "r");
        fread(&bufferImage, height, width, pFile);

        for (int i = 0; i < loadedImage.GetWidth(); i++) {
            for (int j = 0; j < loadedImage.GetHeight(); j++) {
                // SAVE THE RGB VALUES
                loadedImage.SetRGB(i, j, (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j]);
            }
        }

        loadedImage = loadedImage.Rotate90(true);
    }

    printf("Done\n");
    Refresh();
}

void MyFrame::OnConvertRawToJPG(wxCommandEvent& WXUNUSED(event))
{

    printf("Opening Raw Image...\n");
    wxFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), RAW_FILETYPE, wxFD_OPEN, wxDefaultPosition);

    if (openFileDialog.ShowModal() == wxID_OK) {
        wxString name = openFileDialog.GetFilename();
        wxString path = openFileDialog.GetPath();

        string filepath = "../images/";
        string filename = name.ToStdString();
        filepath += filename;

        if (filename == "Cameraman.raw") {
            loadedImage = wxImage(256, 256, true);
        }
        else {
            loadedImage = wxImage(512, 512, true);
        }

        //load raw file
        int height = loadedImage.GetHeight();
        int width = loadedImage.GetWidth();
        unsigned char bufferImage[height][width];
        FILE* pFile;
        pFile = fopen(filepath.c_str(), "r");
        fread(&bufferImage, height, width, pFile);

        for (int i = 0; i < loadedImage.GetWidth(); i++) {
            for (int j = 0; j < loadedImage.GetHeight(); j++) {
                // SAVE THE RGB VALUES
                loadedImage.SetRGB(i, j, (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j]);
            }
        }

        loadedImage = loadedImage.Rotate90(true);

        wxFileDialog
            saveFileDialog(this, _("Save JPG file"), "", "",
                "JPG files (*.jpg)|*.JPG", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_OK) {
            loadedImage.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_JPEG);
        }
    }
    printf("Finished Converting.\n");
}

//###########################################################//
//-----------------------------------------------------------//
//---------- DO NOT MODIFY THE CODE BELOW--------------------//
//-----------------------------------------------------------//
//###########################################################//

//RESET IMAGE
void MyFrame::OnResetImage(wxCommandEvent& WXUNUSED(event))
{

    printf("Reseting image...\n");

    if (loadedImage.Ok()) {
        loadedImage.Destroy();
    }

    if (origImage.Ok()) {
        loadedImage = origImageStack.top();
        origImageStack.push(origImage);
        origImageStack.pop();

    }

    Refresh();

    printf("Finished Reseting.\n");
}

void MyFrame::OnPixelShift(wxCommandEvent& WXUNUSED(event))
{

    printf("Pixel Shifting...\n");
    UseOriginalImageIfNoTemp();
    long pixelShiftValue = 0;
    wxString valueTyped;
    wxTextEntryDialog myDialog(this, _("Enter the pixel shift value (-255 to 255):"), _("Pixel Shift"), _(""));
    if (myDialog.ShowModal() == wxID_OK) {
        valueTyped = myDialog.GetValue();
        if (!valueTyped.ToLong(&pixelShiftValue)) {
            return;
        }
    }
    else {
        return;
    }

    int r, g, b;

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {
            r = tmpImage.GetRed(i, j); // red pixel value
            g = tmpImage.GetGreen(i, j); // green pixel value
            b = tmpImage.GetBlue(i, j); // blue pixel value

            r += pixelShiftValue;
            g += pixelShiftValue;
            b += pixelShiftValue;

            r = (r > 255) ? 255 : r;
            g = (g > 255) ? 255 : g;
            b = (b > 255) ? 255 : b;

            r = (r < 0) ? 0 : r;
            g = (g < 0) ? 0 : g;
            b = (b < 0) ? 0 : b;

            loadedImage.SetRGB(i, j, r,
                g,
                b);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf(" Finished pixel shifting.\n");
    Refresh();
}

void MyFrame::OnAverageFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Average Filter..\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { 1.0, 1.0, 1.0 },
        { 1.0, 1.0, 1.0 },
        { 1.0, 1.0, 1.0 },
    };
    float maskMultiplication = (float)1 / (float)9;
    float sum = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            sum += mask[i][j];
            mask[i][j] = mask[i][j] * maskMultiplication;
        }
    }

    for (int i = mouseDownX+1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY+1; j < mouseUpY - 1; j++) {

            double average = 0;

            average += tmpImage.GetRed(i - 1, j - 1);
            average += tmpImage.GetRed(i - 1, j);
            average += tmpImage.GetRed(i - 1, j + 1);
            average += tmpImage.GetRed(i, j - 1);
            average += tmpImage.GetRed(i, j);
            average += tmpImage.GetRed(i, j + 1);
            average += tmpImage.GetRed(i + 1, j - 1);
            average += tmpImage.GetRed(i + 1, j);
            average += tmpImage.GetRed(i + 1, j + 1);
            float red = average / sum;

            average = 0;

            average += tmpImage.GetGreen(i - 1, j - 1);
            average += tmpImage.GetGreen(i - 1, j);
            average += tmpImage.GetGreen(i - 1, j + 1);
            average += tmpImage.GetGreen(i, j - 1);
            average += tmpImage.GetGreen(i, j);
            average += tmpImage.GetGreen(i, j + 1);
            average += tmpImage.GetGreen(i + 1, j - 1);
            average += tmpImage.GetGreen(i + 1, j);
            average += tmpImage.GetGreen(i + 1, j + 1);
            float green = average / sum;

            average = 0;

            average += tmpImage.GetBlue(i - 1, j - 1);
            average += tmpImage.GetBlue(i - 1, j);
            average += tmpImage.GetBlue(i - 1, j + 1);
            average += tmpImage.GetBlue(i, j - 1);
            average += tmpImage.GetBlue(i, j);
            average += tmpImage.GetBlue(i, j + 1);
            average += tmpImage.GetBlue(i + 1, j - 1);
            average += tmpImage.GetBlue(i + 1, j);
            average += tmpImage.GetBlue(i + 1, j + 1);
            float blue = average / sum;

            loadedImage.SetRGB(i, j, red,
                green,
                blue);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf(" Finished Average Filter.\n");
    Refresh();
}

void MyFrame::OnWeightedAverageFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Average Weighted Filter..\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { 1.0, 2.0, 1.0 },
        { 2.0, 4.0, 2.0 },
        { 1.0, 2.0, 1.0 },
    };

    float maskMultiplication = (float)1 / (float)16;
    float sum = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            sum += mask[i][j];
            mask[i][j] = mask[i][j] * maskMultiplication;
        }
    }

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            double average = 0;

            average += tmpImage.GetRed(i - 1, j - 1);
            average += tmpImage.GetRed(i - 1, j);
            average += tmpImage.GetRed(i - 1, j + 1);
            average += tmpImage.GetRed(i, j - 1);
            average += tmpImage.GetRed(i, j);
            average += tmpImage.GetRed(i, j + 1);
            average += tmpImage.GetRed(i + 1, j - 1);
            average += tmpImage.GetRed(i + 1, j);
            average += tmpImage.GetRed(i + 1, j + 1);
            float red = average / sum;

            average = 0;

            average += tmpImage.GetGreen(i - 1, j - 1);
            average += tmpImage.GetGreen(i - 1, j);
            average += tmpImage.GetGreen(i - 1, j + 1);
            average += tmpImage.GetGreen(i, j - 1);
            average += tmpImage.GetGreen(i, j);
            average += tmpImage.GetGreen(i, j + 1);
            average += tmpImage.GetGreen(i + 1, j - 1);
            average += tmpImage.GetGreen(i + 1, j);
            average += tmpImage.GetGreen(i + 1, j + 1);
            float green = average / sum;

            average = 0;

            average += tmpImage.GetBlue(i - 1, j - 1);
            average += tmpImage.GetBlue(i - 1, j);
            average += tmpImage.GetBlue(i - 1, j + 1);
            average += tmpImage.GetBlue(i, j - 1);
            average += tmpImage.GetBlue(i, j);
            average += tmpImage.GetBlue(i, j + 1);
            average += tmpImage.GetBlue(i + 1, j - 1);
            average += tmpImage.GetBlue(i + 1, j);
            average += tmpImage.GetBlue(i + 1, j + 1);
            float blue = average / sum;

            loadedImage.SetRGB(i, j, red,
                green,
                blue);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf(" Finished Weighted Average Filter.\n");
    Refresh();
}

void MyFrame::On4NeighbourFilter(wxCommandEvent& WXUNUSED(event))
{
    printf("Starting on 4 neighbour filter...\n");

    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { 0, -1.0, 0 },
        { -1.0, 4, -1.0 },
        { 0, -1.0, 0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    printf("Finished 4 neighbour filter.\n");
    Refresh();
}

void MyFrame::On8NeighbourFilter(wxCommandEvent& WXUNUSED(event))
{
    printf("Starting on 8 neighbour filter...\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { -1.0, -1.0, -1.0 },
        { -1.0, 8.0, -1.0 },
        { -1.0, -1.0, -1.0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished 8 neighbour filter.\n");
    Refresh();
}

void MyFrame::On4NeighbourLaplacianEnhancementFilter(wxCommandEvent& WXUNUSED(event))
{
    printf("Starting on 4 neighbour laplacian filter...\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { 0, -1.0, 0 },
        { -1.0, 5.0, -1.0 },
        { 0, -1.0, 0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished 4 neighbour laplacian filter.\n");
    Refresh();
}

void MyFrame::On8NeighbourLaplacianEnhancementFilter(wxCommandEvent& WXUNUSED(event))
{
    printf("Starting on 8 neighbour laplacian filter...\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { -1.0, -1.0, -1.0 },
        { -1.0, 9.0, -1.0 },
        { -1.0, -1.0, -1.0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished 8 neighbour laplacian filter.\n");
    Refresh();
}

void MyFrame::OnRobertsFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Roberts Filter...\n");
    UseOriginalImageIfNoTemp();

    float mask1[3][3] = {
        { 0, 0, 0 },
        { 0, 0, -1.0 },
        { 0, 1.0, 0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask1[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask1[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask1[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask1[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask1[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask1[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask1[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask1[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask1[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask1[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask1[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask1[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask1[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask1[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask1[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask1[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask1[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask1[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask1[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask1[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask1[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask1[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask1[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask1[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask1[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask1[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask1[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    float mask2[3][3] = {
        { 0, 0, 0 },
        { 0, -1.0, 0 },
        { 0, 0, 1.0 },
    };

    UseOriginalImageIfNoTemp();

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask2[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask2[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask2[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask2[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask2[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask2[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask2[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask2[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask2[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask2[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask2[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask2[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask2[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask2[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask2[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask2[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask2[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask2[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask2[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask2[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask2[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask2[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask2[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask2[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask2[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask2[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask2[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Roberts Filter.\n");
    Refresh();
}

void MyFrame::OnSobelXFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Sobel X Filter...\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { -1.0, 0, 1 },
        { -2.0, 0, 2.0 },
        { -1.0, 0, 1.0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Sobel X Filter.\n");
    Refresh();
}

void MyFrame::OnSobelYFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Sobel Y Filter...\n");
    UseOriginalImageIfNoTemp();

    float mask[3][3] = {
        { -1.0, -2.0, -1.0 },
        { 0, 0, 0 },
        { 1.0, 2.0, 1.0 },
    };

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            float sumR = 0;

            sumR += (float)tmpImage.GetRed(i - 1, j - 1) * mask[0][0];
            sumR += (float)tmpImage.GetRed(i - 1, j) * mask[0][1];
            sumR += (float)tmpImage.GetRed(i - 1, j + 1) * mask[0][2];
            sumR += (float)tmpImage.GetRed(i, j - 1) * mask[1][0];
            sumR += (float)tmpImage.GetRed(i, j) * mask[1][1];
            sumR += (float)tmpImage.GetRed(i, j + 1) * mask[1][2];
            sumR += (float)tmpImage.GetRed(i + 1, j - 1) * mask[2][0];
            sumR += (float)tmpImage.GetRed(i + 1, j) * mask[2][1];
            sumR += (float)tmpImage.GetRed(i + 1, j + 1) * mask[2][2];
            sumR = (sumR < 0) ? 0 : sumR;
            sumR = (sumR > 255) ? 255 : sumR;

            float sumG = 0;

            sumG += (float)tmpImage.GetGreen(i - 1, j - 1) * mask[0][0];
            sumG += (float)tmpImage.GetGreen(i - 1, j) * mask[0][1];
            sumG += (float)tmpImage.GetGreen(i - 1, j + 1) * mask[0][2];
            sumG += (float)tmpImage.GetGreen(i, j - 1) * mask[1][0];
            sumG += (float)tmpImage.GetGreen(i, j) * mask[1][1];
            sumG += (float)tmpImage.GetGreen(i, j + 1) * mask[1][2];
            sumG += (float)tmpImage.GetGreen(i + 1, j - 1) * mask[2][0];
            sumG += (float)tmpImage.GetGreen(i + 1, j) * mask[2][1];
            sumG += (float)tmpImage.GetGreen(i + 1, j + 1) * mask[2][2];
            sumG = (sumG < 0) ? 0 : sumG;
            sumG = (sumG > 255) ? 255 : sumG;

            float sumB = 0;

            sumB += (float)tmpImage.GetBlue(i - 1, j - 1) * mask[0][0];
            sumB += (float)tmpImage.GetBlue(i - 1, j) * mask[0][1];
            sumB += (float)tmpImage.GetBlue(i - 1, j + 1) * mask[0][2];
            sumB += (float)tmpImage.GetBlue(i, j - 1) * mask[1][0];
            sumB += (float)tmpImage.GetBlue(i, j) * mask[1][1];
            sumB += (float)tmpImage.GetBlue(i, j + 1) * mask[1][2];
            sumB += (float)tmpImage.GetBlue(i + 1, j - 1) * mask[2][0];
            sumB += (float)tmpImage.GetBlue(i + 1, j) * mask[2][1];
            sumB += (float)tmpImage.GetBlue(i + 1, j + 1) * mask[2][2];
            sumB = (sumB < 0) ? 0 : sumB;
            sumB = (sumB > 255) ? 255 : sumB;

            loadedImage.SetRGB(i, j, sumR,
                sumG,
                sumB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Sobel Y Filter.\n");
    Refresh();
}

void MyFrame::OnSaltAndPepper(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Salt And Pepper...");

    srand(time(NULL));

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {
            int random_number = rand() % 50; // rand() return a number between ​0​ and 49
            if (random_number == 0) {
                loadedImage.SetRGB(i, j, 0, 0, 0);
            }
            if (random_number == 49) {
                loadedImage.SetRGB(i, j, 255, 255, 255);
            }
        }
    }
    ResetDimensionsToOriginal();
    printf("Finished Applying Salt And Pepper.\n");
    Refresh();
}

void MyFrame::OnMinFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Min Filter...");
    UseOriginalImageIfNoTemp();
    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {

            unsigned char r = 255;

            r = min(tmpImage.GetRed(i - 1, j - 1), r);
            r = min(tmpImage.GetRed(i - 1, j), r);
            r = min(tmpImage.GetRed(i - 1, j + 1), r);
            r = min(tmpImage.GetRed(i, j - 1), r);
            r = min(tmpImage.GetRed(i, j), r);
            r = min(tmpImage.GetRed(i, j + 1), r);
            r = min(tmpImage.GetRed(i + 1, j - 1), r);
            r = min(tmpImage.GetRed(i + 1, j), r);
            r = min(tmpImage.GetRed(i + 1, j + 1), r);

            unsigned char g = 255;

            g = min(tmpImage.GetGreen(i - 1, j - 1), g);
            g = min(tmpImage.GetGreen(i - 1, j), g);
            g = min(tmpImage.GetGreen(i - 1, j + 1), g);
            g = min(tmpImage.GetGreen(i, j - 1), g);
            g = min(tmpImage.GetGreen(i, j), g);
            g = min(tmpImage.GetGreen(i, j + 1), g);
            g = min(tmpImage.GetGreen(i + 1, j - 1), g);
            g = min(tmpImage.GetGreen(i + 1, j), g);
            g = min(tmpImage.GetGreen(i + 1, j + 1), g);

            unsigned char b = 255;

            b = min(tmpImage.GetBlue(i - 1, j - 1), b);
            b = min(tmpImage.GetBlue(i - 1, j), b);
            b = min(tmpImage.GetBlue(i - 1, j + 1), b);
            b = min(tmpImage.GetBlue(i, j - 1), b);
            b = min(tmpImage.GetBlue(i, j), b);
            b = min(tmpImage.GetBlue(i, j + 1), b);
            b = min(tmpImage.GetBlue(i + 1, j - 1), b);
            b = min(tmpImage.GetBlue(i + 1, j), b);
            b = min(tmpImage.GetBlue(i + 1, j + 1), b);

            loadedImage.SetRGB(i, j, r, g, b);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Min Filter.\n");
    Refresh();
}

void MyFrame::OnMaxFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Max Filter...");
    UseOriginalImageIfNoTemp();

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY -1; j++) {

            unsigned char r = 0;

            r = max(tmpImage.GetRed(i - 1, j - 1), r);
            r = max(tmpImage.GetRed(i - 1, j), r);
            r = max(tmpImage.GetRed(i - 1, j + 1), r);
            r = max(tmpImage.GetRed(i, j - 1), r);
            r = max(tmpImage.GetRed(i, j), r);
            r = max(tmpImage.GetRed(i, j + 1), r);
            r = max(tmpImage.GetRed(i + 1, j - 1), r);
            r = max(tmpImage.GetRed(i + 1, j), r);
            r = max(tmpImage.GetRed(i + 1, j + 1), r);

            unsigned char g = 0;

            g = max(tmpImage.GetGreen(i - 1, j - 1), g);
            g = max(tmpImage.GetGreen(i - 1, j), g);
            g = max(tmpImage.GetGreen(i - 1, j + 1), g);
            g = max(tmpImage.GetGreen(i, j - 1), g);
            g = max(tmpImage.GetGreen(i, j), g);
            g = max(tmpImage.GetGreen(i, j + 1), g);
            g = max(tmpImage.GetGreen(i + 1, j - 1), g);
            g = max(tmpImage.GetGreen(i + 1, j), g);
            g = max(tmpImage.GetGreen(i + 1, j + 1), g);

            unsigned char b = 0;

            b = max(tmpImage.GetBlue(i - 1, j - 1), b);
            b = max(tmpImage.GetBlue(i - 1, j), b);
            b = max(tmpImage.GetBlue(i - 1, j + 1), b);
            b = max(tmpImage.GetBlue(i, j - 1), b);
            b = max(tmpImage.GetBlue(i, j), b);
            b = max(tmpImage.GetBlue(i, j + 1), b);
            b = max(tmpImage.GetBlue(i + 1, j - 1), b);
            b = max(tmpImage.GetBlue(i + 1, j), b);
            b = max(tmpImage.GetBlue(i + 1, j + 1), b);

            loadedImage.SetRGB(i, j, r, g, b);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Max Filter.\n");
    Refresh();
}

void MyFrame::OnMidpointFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Midpoint Filter...");
    UseOriginalImageIfNoTemp();

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY -1; j++) {

            unsigned char maxPixel = 0;
            unsigned char minPixel = 255;

            maxPixel = max(tmpImage.GetRed(i - 1, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetRed(i - 1, j), maxPixel);
            maxPixel = max(tmpImage.GetRed(i - 1, j + 1), maxPixel);
            maxPixel = max(tmpImage.GetRed(i, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetRed(i, j), maxPixel);
            maxPixel = max(tmpImage.GetRed(i, j + 1), maxPixel);
            maxPixel = max(tmpImage.GetRed(i + 1, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetRed(i + 1, j), maxPixel);
            maxPixel = max(tmpImage.GetRed(i + 1, j + 1), maxPixel);

            minPixel = min(tmpImage.GetRed(i - 1, j - 1), minPixel);
            minPixel = min(tmpImage.GetRed(i - 1, j), minPixel);
            minPixel = min(tmpImage.GetRed(i - 1, j + 1), minPixel);
            minPixel = min(tmpImage.GetRed(i, j - 1), minPixel);
            minPixel = min(tmpImage.GetRed(i, j), minPixel);
            minPixel = min(tmpImage.GetRed(i, j + 1), minPixel);
            minPixel = min(tmpImage.GetRed(i + 1, j - 1), minPixel);
            minPixel = min(tmpImage.GetRed(i + 1, j), minPixel);
            minPixel = min(tmpImage.GetRed(i + 1, j + 1), minPixel);

            unsigned char r = ((int)maxPixel + (int)minPixel) / 2;

            maxPixel = 0;
            minPixel = 255;

            maxPixel = max(tmpImage.GetGreen(i - 1, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i - 1, j), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i - 1, j + 1), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i, j), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i, j + 1), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i + 1, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i + 1, j), maxPixel);
            maxPixel = max(tmpImage.GetGreen(i + 1, j + 1), maxPixel);

            minPixel = min(tmpImage.GetGreen(i - 1, j - 1), minPixel);
            minPixel = min(tmpImage.GetGreen(i - 1, j), minPixel);
            minPixel = min(tmpImage.GetGreen(i - 1, j + 1), minPixel);
            minPixel = min(tmpImage.GetGreen(i, j - 1), minPixel);
            minPixel = min(tmpImage.GetGreen(i, j), minPixel);
            minPixel = min(tmpImage.GetGreen(i, j + 1), minPixel);
            minPixel = min(tmpImage.GetGreen(i + 1, j - 1), minPixel);
            minPixel = min(tmpImage.GetGreen(i + 1, j), minPixel);
            minPixel = min(tmpImage.GetGreen(i + 1, j + 1), minPixel);

            unsigned char g = ((int)maxPixel + (int)minPixel) / 2;

            maxPixel = 0;
            minPixel = 255;

            maxPixel = max(tmpImage.GetBlue(i - 1, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i - 1, j), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i - 1, j + 1), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i, j), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i, j + 1), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i + 1, j - 1), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i + 1, j), maxPixel);
            maxPixel = max(tmpImage.GetBlue(i + 1, j + 1), maxPixel);

            minPixel = min(tmpImage.GetBlue(i - 1, j - 1), minPixel);
            minPixel = min(tmpImage.GetBlue(i - 1, j), minPixel);
            minPixel = min(tmpImage.GetBlue(i - 1, j + 1), minPixel);
            minPixel = min(tmpImage.GetBlue(i, j - 1), minPixel);
            minPixel = min(tmpImage.GetBlue(i, j), minPixel);
            minPixel = min(tmpImage.GetBlue(i, j + 1), minPixel);
            minPixel = min(tmpImage.GetBlue(i + 1, j - 1), minPixel);
            minPixel = min(tmpImage.GetBlue(i + 1, j), minPixel);
            minPixel = min(tmpImage.GetBlue(i + 1, j + 1), minPixel);

            unsigned char b = ((int)maxPixel + (int)minPixel) / 2;

            loadedImage.SetRGB(i, j, r, g, b);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Midpoint Filter.\n");
    Refresh();
}

void MyFrame::OnMedianFilter(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Median Filter...");
    UseOriginalImageIfNoTemp();

    for (int i = mouseDownX + 1; i < mouseUpX - 1; i++) {
        for (int j = mouseDownY + 1; j < mouseUpY - 1; j++) {
            unsigned char pixels[9];

            pixels[0] = tmpImage.GetRed(i - 1, j - 1);
            pixels[1] = tmpImage.GetRed(i - 1, j);
            pixels[2] = tmpImage.GetRed(i - 1, j + 1);
            pixels[3] = tmpImage.GetRed(i, j - 1);
            pixels[4] = tmpImage.GetRed(i, j);
            pixels[5] = tmpImage.GetRed(i, j + 1);
            pixels[6] = tmpImage.GetRed(i + 1, j - 1);
            pixels[7] = tmpImage.GetRed(i + 1, j);
            pixels[8] = tmpImage.GetRed(i + 1, j + 1);

            sort(pixels, pixels + sizeof pixels / sizeof pixels[0]);

            unsigned char r = pixels[4];

            pixels[0] = tmpImage.GetGreen(i - 1, j - 1);
            pixels[1] = tmpImage.GetGreen(i - 1, j);
            pixels[2] = tmpImage.GetGreen(i - 1, j + 1);
            pixels[3] = tmpImage.GetGreen(i, j - 1);
            pixels[4] = tmpImage.GetGreen(i, j);
            pixels[5] = tmpImage.GetGreen(i, j + 1);
            pixels[6] = tmpImage.GetGreen(i + 1, j - 1);
            pixels[7] = tmpImage.GetGreen(i + 1, j);
            pixels[8] = tmpImage.GetGreen(i + 1, j + 1);

            sort(pixels, pixels + sizeof pixels / sizeof pixels[0]);

            unsigned char g = pixels[4];

            pixels[0] = tmpImage.GetBlue(i - 1, j - 1);
            pixels[1] = tmpImage.GetBlue(i - 1, j);
            pixels[2] = tmpImage.GetBlue(i - 1, j + 1);
            pixels[3] = tmpImage.GetBlue(i, j - 1);
            pixels[4] = tmpImage.GetBlue(i, j);
            pixels[5] = tmpImage.GetBlue(i, j + 1);
            pixels[6] = tmpImage.GetBlue(i + 1, j - 1);
            pixels[7] = tmpImage.GetBlue(i + 1, j);
            pixels[8] = tmpImage.GetBlue(i + 1, j + 1);

            sort(pixels, pixels + sizeof pixels / sizeof pixels[0]);

            unsigned char b = pixels[4];

            loadedImage.SetRGB(i, j, r, g, b);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Median Filter.\n");
    Refresh();
}

void MyFrame::OnNegative(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Negative Filter...");
    UseOriginalImageIfNoTemp();
    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {
            loadedImage.SetRGB(i, j, 255 - tmpImage.GetRed(i,j), 255 - tmpImage.GetGreen(i,j), 255 - tmpImage.GetBlue(i,j));
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Negative Filter.\n");
    Refresh();
}

void MyFrame::OnLog(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Log Filter...");

    long constant = 0;
    wxString valueTyped;
    wxTextEntryDialog myDialog(this, _("Enter the Constant Value:"), _("Log Filter"), _(""));

    if (myDialog.ShowModal() == wxID_OK) {
        valueTyped = myDialog.GetValue();
        if (!valueTyped.ToLong(&constant)) {
            return;
        }
    }
    else {
        return;
    }


    UseOriginalImageIfNoTemp();

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {
            unsigned char r = tmpImage.GetRed(i, j);
            long newR = constant * log(r + 1);
            newR = (newR > 255) ? 255 : newR;

            unsigned char g = tmpImage.GetGreen(i, j);
            long newG = constant * log(g + 1);
            newG = (newG > 255) ? 255 : newG;

            unsigned char b = tmpImage.GetBlue(i, j);
            long newB  = constant * log(b + 1);
            newB = (newB > 255) ? 255 : newR;

            loadedImage.SetRGB(i, j, newR, newG, newB);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Log Filter.\n");
    Refresh();
}

void MyFrame::OnPower(wxCommandEvent& WXUNUSED(event))
{
  long constant = 0;
  wxString valueTyped;
  wxTextEntryDialog constantDialog(this, _("Enter the Constant Value:"), _("pow Filter"), _(""));
  if (constantDialog.ShowModal() == wxID_OK) {
      valueTyped = constantDialog.GetValue();
      if (!valueTyped.ToLong(&constant)) {
          return;
      }
  }
  else {
      return;
  }

  double gamma = 0;
  wxTextEntryDialog gammaDialog(this, _("Enter the Gamma Value:"), _("Pow Filter"), _(""));
  if (gammaDialog.ShowModal() == wxID_OK) {
      valueTyped = gammaDialog.GetValue();
      if (!valueTyped.ToDouble(&gamma)) {
          return;
      }
  }
  else {
      return;
  }

    printf("Applying Pow Filter...");
    UseOriginalImageIfNoTemp();

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

            unsigned char r = tmpImage.GetRed(i, j);
            long newR = constant * pow((r), gamma);
            newR = (newR > 255) ? 255 : newR;

            unsigned char g = tmpImage.GetGreen(i, j);
            long newG = constant * pow((g), gamma);
            newG = (newG > 255) ? 255 : newG;

            unsigned char b = tmpImage.GetBlue(i, j);
            long newB  = constant * pow((b), gamma);
            newB = (newB > 255) ? 255 : newR;

            loadedImage.SetRGB(i, j, newR, newG, newB);

        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Pow Filter.\n");
    Refresh();
}

void MyFrame::OnRandomLookupTable(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Random Lookup Table...");
    UseOriginalImageIfNoTemp();
    map<unsigned char, unsigned char> redPixelMap;
    map<unsigned char, unsigned char> greenPixelMap;
    map<unsigned char, unsigned char> bluePixelMap;
    srand(time(NULL));

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {
            unsigned char r,g,b;

            r = tmpImage.GetRed(i,j);
            g = tmpImage.GetGreen(i,j);
            b = tmpImage.GetBlue(i,j);

            if(redPixelMap.count(r) == 0) {
              redPixelMap[r] = rand() % 256;
            }

            if(greenPixelMap.count(g) == 0) {
              greenPixelMap[g] = rand() % 256;
            }
            if(bluePixelMap.count(b) == 0) {
              bluePixelMap[b] = rand() % 256;
            }

            loadedImage.SetRGB(i, j, redPixelMap[r], greenPixelMap[g], bluePixelMap[b]);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Random Lookup Table.\n");
    Refresh();
}

void MyFrame::OnHistogramEqualize(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Histogram Equalization...");
    UseOriginalImageIfNoTemp();
    map<int, double> redPixelMap;
    map<int, double> greenPixelMap;
    map<int, double> bluePixelMap;

    int count = 0;

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

            int r = tmpImage.GetRed(i, j);
            int g = tmpImage.GetGreen(i, j);
            int b = tmpImage.GetBlue(i, j);

            redPixelMap[r] = (redPixelMap.count(r) > 0) ? redPixelMap[r] + 1 : 1;
            greenPixelMap[g] = (greenPixelMap.count(g) > 0) ? greenPixelMap[g] + 1 : 1;
            bluePixelMap[b] = (bluePixelMap.count(b) > 0) ? bluePixelMap[b] + 1 : 1;

            count++;
        }
    }

    int redPixels = redPixelMap.size() - 1;
    int greenPixels = greenPixelMap.size() - 1;
    int bluePixels = bluePixelMap.size() - 1;

    double cumalative = 0;

    //red
    typedef std::map<int, double>::iterator it_type;
    for (it_type iterator = redPixelMap.begin(); iterator != redPixelMap.end(); iterator++) {
        cumalative += iterator->second;
        iterator->second = round((cumalative * redPixels) / count);
    }

    cumalative = 0;
    //green
    for (it_type iterator = greenPixelMap.begin(); iterator != greenPixelMap.end(); iterator++) {
        cumalative += iterator->second;
        iterator->second = round((cumalative * greenPixels) / count);
    }

    cumalative = 0;
    //blue
    for (it_type iterator = bluePixelMap.begin(); iterator != bluePixelMap.end(); iterator++) {
        cumalative += iterator->second;
        iterator->second = round((cumalative * bluePixels) / count);
    }

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

            int r = tmpImage.GetRed(i, j);
            int g = tmpImage.GetGreen(i, j);
            int b = tmpImage.GetBlue(i, j);

            int newR = (redPixelMap.count(r) > 0) ? redPixelMap[r] : r;
            int newG = (greenPixelMap.count(g) > 0) ? greenPixelMap[g] : g;
            int newB = (bluePixelMap.count(b) > 0) ? bluePixelMap[b] : b;

            loadedImage.SetRGB(i, j, newR, newG, newB);

        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Histogram Equalization.\n");
    Refresh();
}

void MyFrame::OnMeanAndStandardDeviation(wxCommandEvent& WXUNUSED(event))
{

    printf("Finding Mean And Standard Deviation...\n");
    UseOriginalImageIfNoTemp();
    map<unsigned char, unsigned char> pixelValMap;

    int count = 0;

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

            int originalPixel = tmpImage.GetRed(i, j);

            if (pixelValMap.count(originalPixel) > 0) {
                pixelValMap[originalPixel] = pixelValMap[originalPixel] + 1;
            }
            else {
                pixelValMap[originalPixel] = 1;
            }

            count++;
        }
    }

    double mean = 0;

    typedef std::map<unsigned char, unsigned char>::iterator it_type;
    for (it_type iterator = pixelValMap.begin(); iterator != pixelValMap.end(); iterator++) {
        mean += (iterator->first * iterator->second);
    }

    mean = mean / count;
    printf("Mean is:%f \n", mean);

    double standardDeviation = 0;

    for (it_type iterator = pixelValMap.begin(); iterator != pixelValMap.end(); iterator++) {
        standardDeviation += pow(iterator->first - mean, 2);
    }

    standardDeviation *= 1.0 / (double)count;

    printf("Standard Deviation is:%f \n", standardDeviation);
    Refresh();
}

void MyFrame::OnSimpleThresholding(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Simple Thresholding\n");
    UseOriginalImageIfNoTemp();
    long threshold = 0;
    wxString valueTyped;
    wxTextEntryDialog myDialog(this, _("Enter the threshold value (0 to 255):"), _("Simple Threshold Value"), _(""));
    if (myDialog.ShowModal() == wxID_OK) {
        valueTyped = myDialog.GetValue();
        if (!valueTyped.ToLong(&threshold)) {
            return;
        }
    }
    else {
        return;
    }

    unsigned char r, g, b;

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

            r = tmpImage.GetRed(i, j);
            g = tmpImage.GetGreen(i, j);
            b = tmpImage.GetBlue(i, j);

            r = threshold > r ? 255 : 0;
            g = threshold > g ? 255 : 0;
            b = threshold > b ? 255 : 0;

            loadedImage.SetRGB(i, j, r, g, b);
        }
    }
    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Simple Thresholding.\n");
    Refresh();
}

void MyFrame::OnAutomatedThresholding(wxCommandEvent& WXUNUSED(event))
{

    printf("Applying Automated Thresholding...\n");

    UseOriginalImageIfNoTemp();
    map<unsigned char, unsigned char> redPixelMap;
    map<unsigned char, unsigned char> greenPixelMap;
    map<unsigned char, unsigned char> bluePixelMap;

    int count = 0;

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

          int r = tmpImage.GetRed(i, j);
          int g = tmpImage.GetGreen(i, j);
          int b = tmpImage.GetBlue(i, j);


          redPixelMap[r] = (redPixelMap.count(r) > 0) ? redPixelMap[r] + 1 : 1;
          greenPixelMap[r] = (greenPixelMap.count(g) > 0) ? greenPixelMap[g] + 1 : 1;
          bluePixelMap[r] = (bluePixelMap.count(b) > 0) ? bluePixelMap[b] + 1 : 1;

            count++;
        }
    }

    double redMean = 0, greenMean = 0, blueMean = 0;

    typedef std::map<unsigned char, unsigned char>::iterator it_type;

    for (it_type iterator = redPixelMap.begin(); iterator != redPixelMap.end(); iterator++) {
        redMean += (iterator->first * iterator->second);
    }
    for (it_type iterator = greenPixelMap.begin(); iterator != greenPixelMap.end(); iterator++) {
        greenMean += (iterator->first * iterator->second);
    }
    for (it_type iterator = bluePixelMap.begin(); iterator != bluePixelMap.end(); iterator++) {
        blueMean += (iterator->first * iterator->second);
    }

    redMean = redMean / count;
    blueMean = blueMean / count;
    greenMean = greenMean / count;

    double redThreshold = redMean;
    double blueThreshold = blueMean;
    double greenThreshold = greenMean;

    double start = 0;
    double end = 255;
    double prevMean1 = 0;
    double prevMean2 = 0;
    double mean1 = 1;
    double mean2 = 1;


    while (prevMean1 != mean1 && prevMean2 != mean2) {
        prevMean1 = mean1;
        prevMean2 = mean2;
        mean1 = getMeanOfHistogramRange(redPixelMap, start, redThreshold);
        mean2 = getMeanOfHistogramRange(redPixelMap, redThreshold, end);
        redThreshold = (mean1 + mean2) / 2;
    }

     prevMean1 = 0;
     prevMean2 = 0;
     mean1 = 1;
     mean2 = 1;


    while (prevMean1 != mean1 && prevMean2 != mean2) {
        prevMean1 = mean1;
        prevMean2 = mean2;
        mean1 = getMeanOfHistogramRange(greenPixelMap, start, greenThreshold);
        mean2 = getMeanOfHistogramRange(greenPixelMap, greenThreshold, end);
        greenThreshold = (mean1 + mean2) / 2;
    }

    prevMean1 = 0;
    prevMean2 = 0;
    mean1 = 1;
    mean2 = 1;


   while (prevMean1 != mean1 && prevMean2 != mean2) {
       prevMean1 = mean1;
       prevMean2 = mean2;
       mean1 = getMeanOfHistogramRange(bluePixelMap, start, blueThreshold);
       mean2 = getMeanOfHistogramRange(bluePixelMap, blueThreshold, end);
       blueThreshold = (mean1 + mean2) / 2;
   }


    unsigned char r, g, b;

    for (int i = mouseDownX; i < mouseUpX; i++) {
        for (int j = mouseDownY; j < mouseUpY; j++) {

            r = tmpImage.GetRed(i, j);
            g = tmpImage.GetGreen(i, j);
            b = tmpImage.GetBlue(i, j);

            r = redThreshold > r ? 255 : 0;
            g = greenThreshold > g ? 255 : 0;
            b = blueThreshold > b ? 255 : 0;

            loadedImage.SetRGB(i, j, r, g, b);
        }
    }

    tmpImage.Destroy();
    ResetDimensionsToOriginal();
    printf("Finished Applying Automated Thresholding.\n");
    Refresh();
}

double MyFrame::getMeanOfHistogramRange(map<unsigned char, unsigned char> pixelValueMap, double start, double end)
{
    int count = 0;
    double mean = 0;

    typedef std::map<unsigned char, unsigned char>::iterator it_type;
    for (it_type iterator = pixelValueMap.begin(); iterator != pixelValueMap.end(); iterator++) {
        if (iterator->first >= start && iterator->first <= end) {
            count += iterator->second;
            mean += (iterator->first * iterator->second);
        }
    }

    mean = mean / count;
    if(isnan(mean)) {
      return 0;
    }
    return mean;
}

//IMAGE SAVING

void MyFrame::OnMouseDown(wxMouseEvent& event)
{
  if(canSelect) {
    mouseDownX = event.GetPosition().x;
    mouseDownY = event.GetPosition().y;
    event.Skip();
  }

}

void MyFrame::OnMouseUp(wxMouseEvent& event)
{
  if(canSelect) {
  mouseUpX = event.GetPosition().x;
  mouseUpY = event.GetPosition().y;

  if(mouseDownX > loadedImage.GetWidth() || mouseUpX > loadedImage.GetWidth()) {
    wxMessageBox("Please select a region within the image");
    return;
  }

  if(mouseDownY > loadedImage.GetHeight() || mouseUpY > loadedImage.GetHeight()) {
    wxMessageBox("Please select a region within the image");
    return;
  }

    tmpImage = loadedImage.Copy();
    unsigned char r, g, b;

    for(int i = mouseDownX + 1; i <= mouseUpX - 1; i++) {
      for(int j = mouseDownY + 1; j <= mouseUpY - 1; j++) {

          r = loadedImage.GetRed(i, j);
          g = loadedImage.GetGreen(i, j);
          b = loadedImage.GetBlue(i, j);
          if(b + 30 > 255) {
            b = 255;
          } else {
            b += 30;
          }
          loadedImage.SetRGB(i,j, r, g , b);
      }
    }

    menuBar->Enable(true);
    Refresh();
    event.Skip();
    selected = true;
    canSelect = false;
}


}

void MyFrame::OnSelect(wxCommandEvent& WXUNUSED(event))
{

    if(selected) {
      wxMessageBox( wxT("Region of interest is already selected") );
      return;
    }
    printf("Waiting for user to select...");

    wxMessageBox( wxT("Please drag your mouse to select a region of the image") );
    printf("Finished selecting.\n");
    Refresh();
    menuBar->Enable(false);
    canSelect = true;
}

void MyFrame::OnUnSelect(wxCommandEvent& WXUNUSED(event))
{

    printf("Unselected");
    ResetDimensionsToOriginal();
    loadedImage = tmpImage.Copy();
    Refresh();
}



void MyFrame::OnSaveImage(wxCommandEvent& WXUNUSED(event))
{

    printf("Saving image...");

    loadedImage.SaveFile(wxT("Saved_Image.bmp"), wxBITMAP_TYPE_BMP);


    printf("Finished Saving.\n");
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{

    Close(TRUE);
}

void MyFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc(this);
    if (loadedImage.Ok()) {
        undoStack.push(loadedImage.Copy());
        origImageStack.push(origImage.Copy());
        dc.DrawBitmap(wxBitmap(loadedImage), 0, 0, false); //given bitmap xcoord y coord and transparency
    }

}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(LOAD_FILE_ID, MyFrame::OnOpenFile)
EVT_MENU(EXIT_ID, MyFrame::OnExit)

//###########################################################//
//----------------------START MY EVENTS ---------------------//
//###########################################################//

EVT_MENU(RESET_IMAGE_ID, MyFrame::OnResetImage)
EVT_MENU(UNDO, MyFrame::OnUndo)
EVT_MENU(SCALE_IMAGE_ID, MyFrame::OnScaleImage)
EVT_MENU(SAVE_IMAGE_ID, MyFrame::OnSaveImage)
EVT_MENU(PIXEL_SHIFT_ID, MyFrame::OnPixelShift) //--->To be modified!
EVT_MENU(OPEN_RAW_ID, MyFrame::OnOpenRawFile) //--->To be modified!
EVT_MENU(SAVE_RAW_TO_JPG, MyFrame::OnConvertRawToJPG) //--->To be modified!
EVT_MENU(AVERAGE_FILTER_ID, MyFrame::OnAverageFilter) //--->To be modified!
EVT_MENU(WEIGHTED_AVERAGE_FILTER_ID, MyFrame::OnWeightedAverageFilter) //--->To be modified!
EVT_MENU(FOUR_NEIGHBOUR_FILTER_ID, MyFrame::On4NeighbourFilter) //--->To be modified!
EVT_MENU(EIGHT_NEIGHBOUR_FILTER_ID, MyFrame::On8NeighbourFilter) //--->To be modified!
EVT_MENU(FOUR_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID, MyFrame::On4NeighbourLaplacianEnhancementFilter) //--->To be modified!
EVT_MENU(EIGHT_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID, MyFrame::On8NeighbourLaplacianEnhancementFilter) //--->To be modified!
EVT_MENU(ROBERTS_ID, MyFrame::OnRobertsFilter) //--->To be modified!
EVT_MENU(SOBEL_X, MyFrame::OnSobelXFilter) //--->To be modified!
EVT_MENU(SOBEL_Y, MyFrame::OnSobelYFilter) //--->To be modified!
EVT_MENU(SALT_AND_PEPPER_ID, MyFrame::OnSaltAndPepper) //--->To be modified!
EVT_MENU(MIN_ID, MyFrame::OnMinFilter) //--->To be modified!
EVT_MENU(MAX_ID, MyFrame::OnMaxFilter) //--->To be modified!
EVT_MENU(MIDPOINT_ID, MyFrame::OnMidpointFilter) //--->To be modified!
EVT_MENU(MEDIAN_ID, MyFrame::OnMedianFilter) //--->To be modified!
EVT_MENU(NEGATIVE_ID, MyFrame::OnNegative) //--->To be modified!
EVT_MENU(LOG_ID, MyFrame::OnLog) //--->To be modified!
EVT_MENU(POWER_ID, MyFrame::OnPower) //--->To be modified!
EVT_MENU(HISTOGRAM_EQUALIZE, MyFrame::OnHistogramEqualize) //--->To be modified!
EVT_MENU(RANDOM_LOOKUP_TABLE, MyFrame::OnRandomLookupTable) //--->To be modified!
EVT_MENU(MEAN_AND_STANDARD_DEVIATION, MyFrame::OnMeanAndStandardDeviation) //--->To be modified!
EVT_MENU(SIMPLE_THRESHOLDING, MyFrame::OnSimpleThresholding) //--->To be modified!
EVT_MENU(AUTOMATED_THRESHOLDING, MyFrame::OnAutomatedThresholding) //--->To be modified!
EVT_MENU(SELECT, MyFrame::OnSelect) //--->To be modified!
EVT_MENU(UNSELECT, MyFrame::OnUnSelect) //--->To be modified!

EVT_LEFT_DOWN ( MyFrame::OnMouseDown )
EVT_LEFT_UP ( MyFrame::OnMouseUp )



//###########################################################//
//----------------------END MY EVENTS -----------------------//
//###########################################################//

EVT_PAINT(MyFrame::OnPaint)
END_EVENT_TABLE()
