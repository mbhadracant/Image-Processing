#include <wx/wx.h>
#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <iostream>
#include <window.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <math.h>
#include <map>

using namespace std;

static const wxChar *FILETYPES = _T("All files (*.*)|*.*");
static const wxChar *RAW_FILETYPE = _T("RAW files (*.raw)|*.raw");

IMPLEMENT_APP(BasicApplication)

bool BasicApplication::OnInit()
{
    wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame(_("Basic Frame"), 50, 50, 800, 600);
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
}

MyFrame::MyFrame(const wxString title, int xpos, int ypos, int width, int height): wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height)){

    menuBar = new wxMenuBar;


    //###########################################################//
    //----------------------START MY MENU -----------------------//
    //###########################################################//
    fileMenu = new wxMenu;
    fileMenu->Append(LOAD_FILE_ID, _T("&Open file"));
    fileMenu->Append(OPEN_RAW_ID, _T("&Open RAW file")); //--->To be modified!
    fileMenu->Append(SAVE_RAW_TO_JPG, _T("&RAW Image to JPG"));
    fileMenu->AppendSeparator();

    fileMenu->Append(RESET_IMAGE_ID, _T("&Reset image"));
    fileMenu->Append(INVERT_IMAGE_ID, _T("&Invert image"));

    fileMenu->AppendSeparator();
    fileMenu->Append(SAVE_IMAGE_ID, _T("&Save image"));
    fileMenu->Append(EXIT_ID, _T("E&xit"));

    menuBar->Append(fileMenu, _T("&File"));

    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//



    lab4Menu = new wxMenu;


    lab4Menu->Append(SCALE_IMAGE_ID, _T("&Scale image"));

    menuBar->Append(lab4Menu, _T("&Pixel Scaling"));



    //###########################################################//
    //----------------------END MY MENU -------------------------//
    //###########################################################//

    lab5Menu = new wxMenu;
    lab5Menu->Append(PIXEL_SHIFT_ID, _T("&Pixel Shift"));
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

    SetMenuBar(menuBar);
    CreateStatusBar(3);
    loadedImage = wxImage("../images/cateyes.png");
    origImage = loadedImage.Copy();

    SetBackgroundStyle(wxBG_STYLE_PAINT);

}

MyFrame::~MyFrame() {
    /* release resources */
    if(loadedImage.Ok()) {
        loadedImage.Destroy();
    }

    if (origImage.Ok()) {
      origImage.Destroy();
    }

}

void MyFrame::OnOpenFile(wxCommandEvent & WXUNUSED(event)) {

    wxFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), FILETYPES, wxFD_OPEN, wxDefaultPosition);

    if(openFileDialog.ShowModal() == wxID_OK){
        wxString filename = openFileDialog.GetFilename();
        wxString path = openFileDialog.GetPath();
        printf("Loading image form file...");

        if(loadedImage.Ok()) {
            loadedImage.Destroy();
        }

        if (origImage.Ok()) {
            origImage.Destroy();
        }

        loadedImage = wxImage(path); //Image Loaded form file

        if(loadedImage.Ok()) {
            origImage = loadedImage.Copy();
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
void MyFrame::OnInvertImage(wxCommandEvent & WXUNUSED(event)) {

    printf("Inverting...");
    wxImage tmpImage = loadedImage.Copy();
    for(int i = 0; i < loadedImage.GetWidth(); i++) {
        for(int j = 0;j < loadedImage.GetHeight(); j++) {
            loadedImage.SetRGB(i, j, 255 - tmpImage.GetRed(i,j),
                               255 - tmpImage.GetGreen(i,j),
                               255 - tmpImage.GetBlue(i,j));
        }
    }

    tmpImage.Destroy();
    printf(" Finished inverting.\n");
    Refresh();
}

int round_int( double r ) {
    return (r > 0.0) ? (r + 0.5) : (r - 0.5);
}

//IMAGE SCALEING
void MyFrame::OnScaleImage(wxCommandEvent & WXUNUSED(event)) {

    printf("Scaling...\n");
    wxImage tmpImage = loadedImage.Copy();
    float scaleFact = 0;
    double input = 0;
    unsigned char r,g,b;
    //xFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), FILETYPES, wxFD_OPEN, wxDefaultPosition);

    wxString valueTyped;
    wxTextEntryDialog myDialog(this, _("Enter the pixel scale factor:"), _("Pixel Scaling"), _(""));
    if ( myDialog.ShowModal() == wxID_OK )
    {
        valueTyped = myDialog.GetValue();
        if(valueTyped.ToDouble(&input)) {
          scaleFact = (float)input;
        } else {
          cout << "Invalid Input: " << valueTyped << endl;
          return;
        }
    } else {
      return;
    }

    for(int i=0; i < loadedImage.GetWidth(); i++) {
        for(int j=0; j < loadedImage.GetHeight(); j++) {
          r = tmpImage.GetRed(i,j);   // red pixel value
          g = tmpImage.GetGreen(i,j); // green pixel value
          b = tmpImage.GetBlue(i,j); // blue pixel value

        //printf("(%d,%d) [r = %d  | g = %d | b = %d] \n",i,j,(unsigned)r,(unsigned)g,(unsigned)b);
            unsigned char newR, newG, newB;


            if((r > 255 / scaleFact)) {
              newR = 255;
            } else {
              newR = r * scaleFact;
            }
            if((g > 255 / scaleFact)) {
              newG = 255;
            } else {
              newG = g * scaleFact;
            }
            if((b > 255 / scaleFact)) {
              newB = 255;
            } else {
              newB = b * scaleFact;
            }

            loadedImage.SetRGB(i, j, newR,
                               newG,
                               newB);


       }
    }

    tmpImage.Destroy();
    printf(" Finished scaling.\n");
    Refresh();
}

void MyFrame::OnOpenRawFile(wxCommandEvent & WXUNUSED(event)) {
  printf("Opening Raw Image...\n");

  wxFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), RAW_FILETYPE, wxFD_OPEN, wxDefaultPosition);

  if(openFileDialog.ShowModal() == wxID_OK){
      wxString name = openFileDialog.GetFilename();
      wxString path = openFileDialog.GetPath();

      string filepath = "../images/";
      string filename = name.ToStdString();
      filepath += filename;

      if(loadedImage.Ok()) {
          loadedImage.Destroy();
      }

      if (origImage.Ok()) {
          origImage.Destroy();
      }

      if(filename == "Cameraman.raw") {
        loadedImage = wxImage(256,256,true);
      } else {
        loadedImage = wxImage(512,512,true);
      }


      if(loadedImage.Ok()) {
          origImage = loadedImage.Copy();
      }


      //load raw file
      int height = loadedImage.GetHeight();
      int width = loadedImage.GetWidth();
      unsigned char bufferImage[height][width];
      FILE * pFile;
      pFile = fopen(filepath.c_str(), "r");
      fread(&bufferImage, height,width , pFile);


      for(int i=0; i< loadedImage.GetWidth(); i++) {
          for(int j=0;j< loadedImage.GetHeight(); j++){
            // SAVE THE RGB VALUES
            loadedImage.SetRGB(i, j, (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j]);
          }
      }

      loadedImage = loadedImage.Rotate90(true);

    }



  printf("Done\n");
  Refresh();
}

void MyFrame::OnConvertRawToJPG(wxCommandEvent & WXUNUSED(event)) {

  printf("Opening Raw Image...\n");
#include <algorithm>
  wxFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), RAW_FILETYPE, wxFD_OPEN, wxDefaultPosition);

  if(openFileDialog.ShowModal() == wxID_OK){
      wxString name = openFileDialog.GetFilename();
      wxString path = openFileDialog.GetPath();

      string filepath = "../images/";
      string filename = name.ToStdString();
      filepath += filename;

      if(filename == "Cameraman.raw") {
        loadedImage = wxImage(256,256,true);
      } else {
        loadedImage = wxImage(512,512,true);
      }

      //load raw file
      int height = loadedImage.GetHeight();
      int width = loadedImage.GetWidth();
      unsigned char bufferImage[height][width];
      FILE * pFile;
      pFile = fopen(filepath.c_str(), "r");
      fread(&bufferImage, height,width , pFile);


      for(int i=0; i< loadedImage.GetWidth(); i++) {
          for(int j=0;j< loadedImage.GetHeight(); j++){
            // SAVE THE RGB VALUES
            loadedImage.SetRGB(i, j, (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j], (unsigned)bufferImage[i][j]);
          }
      }

      loadedImage = loadedImage.Rotate90(true);

      wxFileDialog
    saveFileDialog(this, _("Save JPG file"), "", "",
                   "JPG files (*.jpg)|*.JPG", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
if (saveFileDialog.ShowModal() == wxID_OK) {
    loadedImage.SaveFile(saveFileDialog.GetPath(),wxBITMAP_TYPE_JPEG);
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
void MyFrame::OnResetImage(wxCommandEvent & WXUNUSED(event)) {

    printf("Reseting image...");
    if (loadedImage.Ok()) {
       loadedImage.Destroy();
    }

    if (origImage.Ok()) {
        loadedImage = origImage.Copy();
        Refresh();
    }

    printf("Finished Reseting.\n");

}


void MyFrame::OnPixelShift(wxCommandEvent & WXUNUSED(event)) {

  printf("Pixel Shifting...\n");
  wxImage tmpImage = loadedImage.Copy();
  long pixelShiftValue = 0;
  unsigned char r,g,b;
  //xFileDialog openFileDialog(this, _T("Open file"), _T(""), _T(""), FILETYPES, wxFD_OPEN, wxDefaultPosition);

  wxString valueTyped;
  wxTextEntryDialog myDialog(this, _("Enter the pixel shift value (-255 to 255):"), _("Pixel Shift"), _(""));
  if ( myDialog.ShowModal() == wxID_OK )
  {
      valueTyped = myDialog.GetValue();
      if(!valueTyped.ToLong(&pixelShiftValue)) {
        return;
      }
  } else {
    return;
  }


  for(int i=0; i < loadedImage.GetWidth(); i++) {
      for(int j=0; j < loadedImage.GetHeight(); j++) {
        r = tmpImage.GetRed(i,j);   // red pixel value
        g = tmpImage.GetGreen(i,j); // green pixel value
        b = tmpImage.GetBlue(i,j); // blue pixel value

      //printf("(%d,%d) [r = %d  | g = %d | b = %d] \n",i,j,(unsigned)r,(unsigned)g,(unsigned)b);
          unsigned char newR, newG, newB;

          if(pixelShiftValue > 0) {
            if ((pixelShiftValue > 0) && (r > 255 - pixelShiftValue)) {
                newR = 255;
                newG = 255;
                newB = 255;
            } else {
              newR = pixelShiftValue + r;
              newG = pixelShiftValue + g;
              newB = pixelShiftValue + b;
            }
          }

          if(pixelShiftValue < 0) {
            if ((pixelShiftValue < 0) && (r < 0 - pixelShiftValue)) {
              newR = 0;
              newG = 0;
              newB = 0;
            } else {
              newR = r + pixelShiftValue;
              newG = g + pixelShiftValue;
              newB = b + pixelShiftValue;
            }
          }

          loadedImage.SetRGB(i, j, newR,
                             newG,
                             newB);


     }
  }

  tmpImage.Destroy();
  printf(" Finished pixel shifting.\n");
  Refresh();
}

void MyFrame::OnAverageFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Average Filter..\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] ={
                     {1.0,1.0,1.0},
                     {1.0,1.0,1.0},
                     {1.0,1.0,1.0},
                   };
  float maskMultiplication = (float)1/(float)9;
  float sum = 0;

  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      sum += mask[i][j];
      mask[i][j] = mask[i][j] * maskMultiplication;
    }
  }


  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {


        double average = 0;

        average += tmpImage.GetRed(i-1,j-1);
        average += tmpImage.GetRed(i-1,j);
        average += tmpImage.GetRed(i-1,j+1);
        average += tmpImage.GetRed(i,j-1);
        average += tmpImage.GetRed(i,j);
        average += tmpImage.GetRed(i,j+1);
        average += tmpImage.GetRed(i+1,j-1);
        average += tmpImage.GetRed(i+1,j);
        average += tmpImage.GetRed(i+1,j+1);
        float value = average/sum;

          loadedImage.SetRGB(i, j, value,
                             value,
                             value);

     }
  }

  tmpImage.Destroy();
  printf(" Finished Average Filter.\n");
  Refresh();
}

void MyFrame::OnWeightedAverageFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Average Weighted Filter..\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] ={
                     {1.0,2.0,1.0},
                     {2.0,4.0,2.0},
                     {1.0,2.0,1.0},
                   };

  float maskMultiplication = (float)1/(float)16;
  float sum = 0;
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      sum += mask[i][j];
      mask[i][j] = mask[i][j] * maskMultiplication;
    }
  }

  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        double average = 0;

        average += tmpImage.GetRed(i-1,j-1);
        average += tmpImage.GetRed(i-1,j);
        average += tmpImage.GetRed(i-1,j+1);
        average += tmpImage.GetRed(i,j-1);
        average += tmpImage.GetRed(i,j);
        average += tmpImage.GetRed(i,j+1);
        average += tmpImage.GetRed(i+1,j-1);
        average += tmpImage.GetRed(i+1,j);
        average += tmpImage.GetRed(i+1,j+1);
        float value = average/sum;

          loadedImage.SetRGB(i, j, value,
                             value,
                             value);

     }
  }

  tmpImage.Destroy();
  printf(" Finished Weighted Average Filter.\n");
  Refresh();
}

void MyFrame::On4NeighbourFilter(wxCommandEvent & WXUNUSED(event)) {
    printf("Starting on 4 neighbour filter...\n");

    wxImage tmpImage = loadedImage.Copy();

    float mask[3][3] ={
                       {0,-1.0,0},
                       {-1.0,4,-1.0},
                       {0,-1.0,0},
                     };



    for(int i=1; i < loadedImage.GetWidth()-1; i++) {
        for(int j=1; j < loadedImage.GetHeight()-1; j++) {


          float sum = 0;

          sum += (float)tmpImage.GetRed(i-1,j-1) * mask[0][0];
          sum += (float)tmpImage.GetRed(i-1,j) * mask[0][1];
          sum += (float)tmpImage.GetRed(i-1,j+1) * mask[0][2];
          sum += (float)tmpImage.GetRed(i,j-1) * mask[1][0];
          sum += (float)tmpImage.GetRed(i,j) * mask[1][1];
          sum += (float)tmpImage.GetRed(i,j+1) * mask[1][2];
          sum += (float)tmpImage.GetRed(i+1,j-1) * mask[2][0];
          sum += (float)tmpImage.GetRed(i+1,j) * mask[2][1];
          sum += (float)tmpImage.GetRed(i+1,j+1) * mask[2][2];
          if(sum < 0) {
            sum = 0;
          }
          if(sum > 255) {
            sum = 255;
          }
            loadedImage.SetRGB(i, j, sum,
                               sum,
                               sum);

       }
    }

    tmpImage.Destroy();
    printf("Finished 4 neighbour filter.\n");
    Refresh();
}

void MyFrame::On8NeighbourFilter(wxCommandEvent & WXUNUSED(event)) {
  printf("Starting on 8 neighbour filter...\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] ={
                     {-1.0,-1.0,-1.0},
                     {-1.0,8.0,-1.0},
                     {-1.0,-1.0,-1.0},
                   };



  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {


        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask[1][1];
        sum += (float)tmpImage.GetRed(i,j+1) * mask[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }

  tmpImage.Destroy();
  printf("Finished 8 neighbour filter.\n");
  Refresh();
}

void MyFrame::On4NeighbourLaplacianEnhancementFilter(wxCommandEvent & WXUNUSED(event)) {
  printf("Starting on 4 neighbour laplacian filter...\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] ={
                     {0,-1.0,0},
                     {-1.0,5.0,-1.0},
                     {0,-1.0,0},
                   };



  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {


        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask[1][1];
        sum += (float)tmpImage.GetRed(i,j+1) * mask[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }

  tmpImage.Destroy();
  printf("Finished 4 neighbour laplacian filter.\n");
  Refresh();
}

void MyFrame::On8NeighbourLaplacianEnhancementFilter(wxCommandEvent & WXUNUSED(event)) {
  printf("Starting on 8 neighbour laplacian filter...\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] ={
                     {-1.0,-1.0,-1.0},
                     {-1.0,9.0,-1.0},
                     {-1.0,-1.0,-1.0},
                   };

   for(int i=1; i < loadedImage.GetWidth()-1; i++) {
       for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask[1][1];
        sum += (float)tmpImage.GetRed(i,j+1) * mask[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }

  tmpImage.Destroy();
  printf("Finished 8 neighbour laplacian filter.\n");
  Refresh();
}

void MyFrame::OnRobertsFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Roberts Filter...\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask1[3][3] = {
                       {0,0,0},
                       {0,0,-1.0},
                       {0,1.0,0},
                     };



  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask1[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask1[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask1[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask1[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask1[1][1];
        sum += (float)tmpImage.GetRed(i,j+1) * mask1[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask1[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask1[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask1[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }


   float mask2[3][3] = {
                       {0,0,0},
                       {0,-1.0,0},
                       {0,0,1.0},
                     };

  tmpImage = loadedImage.Copy();

  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask2[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask2[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask2[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask2[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask2[1][1];
        sum += (float)tmpImage.GetRed(i-1,j) * mask1[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask1[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask1[1][0];
        sum += (float)tmpImage.GetRed(i,j+1) * mask2[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask2[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask2[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask2[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }

  tmpImage.Destroy();
  printf("Finished Applying Roberts Filter.\n");
  Refresh();
}

void MyFrame::OnSobelXFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Sobel X Filter...\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] = {
                       {-1.0,0,1},
                       {-2.0,0,2.0},
                       {-1.0,0,1.0},
                     };



  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask[1][1];
        sum += (float)tmpImage.GetRed(i,j+1) * mask[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }

  tmpImage.Destroy();
  printf("Finished Applying Sobel X Filter.\n");
  Refresh();
}

void MyFrame::OnSobelYFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Sobel Y Filter...\n");
  wxImage tmpImage = loadedImage.Copy();

  float mask[3][3] = {
                       {-1.0,-2.0,-1.0},
                       {0,0,0},
                       {1.0,2.0,1.0},
                     };



  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        float sum = 0;

        sum += (float)tmpImage.GetRed(i-1,j-1) * mask[0][0];
        sum += (float)tmpImage.GetRed(i-1,j) * mask[0][1];
        sum += (float)tmpImage.GetRed(i-1,j+1) * mask[0][2];
        sum += (float)tmpImage.GetRed(i,j-1) * mask[1][0];
        sum += (float)tmpImage.GetRed(i,j) * mask[1][1];
        sum += (float)tmpImage.GetRed(i,j+1) * mask[1][2];
        sum += (float)tmpImage.GetRed(i+1,j-1) * mask[2][0];
        sum += (float)tmpImage.GetRed(i+1,j) * mask[2][1];
        sum += (float)tmpImage.GetRed(i+1,j+1) * mask[2][2];
        if(sum < 0) {
          sum = 0;
        }
        if(sum > 255) {
          sum = 255;
        }
          loadedImage.SetRGB(i, j, sum,
                             sum,
                             sum);

     }
  }

  tmpImage.Destroy();
  printf("Finished Applying Sobel Y Filter.\n");
  Refresh();
}


//(5,3)#include <cstdlib>
#include <ctime>
//(10,4)




void MyFrame::OnSaltAndPepper(wxCommandEvent & WXUNUSED(event)) {

    printf("Applying Salt And Pepper...");


    srand(time(NULL));

    for(int i=0; i < loadedImage.GetWidth(); i++) {
        for(int j=0; j < loadedImage.GetHeight(); j++) {
          int random_number = rand() % 50; // rand() return a number between ​0​ and 19
          cout << random_number << endl;
          if(random_number == 0) {
            loadedImage.SetRGB(i, j, 0,
                               0,
                               0);

          }
          if(random_number == 49) {
            loadedImage.SetRGB(i, j, 255,
                               255,
                               255);
          }




       }
    }


    printf("Finished Applying Salt And Pepper.\n");
    Refresh();
}

void MyFrame::OnMinFilter(wxCommandEvent & WXUNUSED(event)) {

    printf("Applying Min Filter...");
    wxImage tmpImage = loadedImage.Copy();

    for(int i=1; i < loadedImage.GetWidth()-1; i++) {
        for(int j=1; j < loadedImage.GetHeight()-1; j++) {

          unsigned char minPixel = 255;

          minPixel = min(tmpImage.GetRed(i-1,j-1),minPixel);
          minPixel = min(tmpImage.GetRed(i-1,j),minPixel);
          minPixel = min(tmpImage.GetRed(i-1,j+1),minPixel);
          minPixel = min(tmpImage.GetRed(i,j-1),minPixel);
          minPixel = min(tmpImage.GetRed(i,j),minPixel);
          minPixel = min(tmpImage.GetRed(i,j+1),minPixel);
          minPixel = min(tmpImage.GetRed(i+1,j-1),minPixel);
          minPixel = min(tmpImage.GetRed(i+1,j),minPixel);
          minPixel = min(tmpImage.GetRed(i+1,j+1),minPixel);

          loadedImage.SetRGB(i, j, minPixel, minPixel, minPixel);
       }
    }

    tmpImage.Destroy();

    printf("Finished Applying Min Filter.\n");
    Refresh();
}

void MyFrame::OnMaxFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Max Filter...");
  wxImage tmpImage = loadedImage.Copy();

  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        unsigned char maxPixel = 0;

        maxPixel = max(tmpImage.GetRed(i-1,j-1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i-1,j),maxPixel);
        maxPixel = max(tmpImage.GetRed(i-1,j+1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i,j-1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i,j),maxPixel);
        maxPixel = max(tmpImage.GetRed(i,j+1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i+1,j-1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i+1,j),maxPixel);
        maxPixel = max(tmpImage.GetRed(i+1,j+1),maxPixel);

        loadedImage.SetRGB(i, j, maxPixel, maxPixel, maxPixel);
     }
  }

  tmpImage.Destroy();

  printf("Finished Applying Max Filter.\n");
  Refresh();
}

void MyFrame::OnMidpointFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Midpoint Filter...");
  wxImage tmpImage = loadedImage.Copy();

  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {

        unsigned char maxPixel = 0;

        maxPixel = max(tmpImage.GetRed(i-1,j-1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i-1,j),maxPixel);
        maxPixel = max(tmpImage.GetRed(i-1,j+1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i,j-1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i,j),maxPixel);
        maxPixel = max(tmpImage.GetRed(i,j+1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i+1,j-1),maxPixel);
        maxPixel = max(tmpImage.GetRed(i+1,j),maxPixel);
        maxPixel = max(tmpImage.GetRed(i+1,j+1),maxPixel);

        unsigned char minPixel = 255;

        minPixel = min(tmpImage.GetRed(i-1,j-1),minPixel);
        minPixel = min(tmpImage.GetRed(i-1,j),minPixel);
        minPixel = min(tmpImage.GetRed(i-1,j+1),minPixel);
        minPixel = min(tmpImage.GetRed(i,j-1),minPixel);
        minPixel = min(tmpImage.GetRed(i,j),minPixel);
        minPixel = min(tmpImage.GetRed(i,j+1),minPixel);
        minPixel = min(tmpImage.GetRed(i+1,j-1),minPixel);
        minPixel = min(tmpImage.GetRed(i+1,j),minPixel);
        minPixel = min(tmpImage.GetRed(i+1,j+1),minPixel);

        unsigned char midpoint = ((int)maxPixel + (int)minPixel)/2;

        loadedImage.SetRGB(i, j, midpoint, midpoint, midpoint);
     }
  }

  tmpImage.Destroy();

  printf("Finished Midpoint Filter.\n");
  Refresh();
}

void MyFrame::OnMedianFilter(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Max Filter...");
  wxImage tmpImage = loadedImage.Copy();

  for(int i=1; i < loadedImage.GetWidth()-1; i++) {
      for(int j=1; j < loadedImage.GetHeight()-1; j++) {
        unsigned char pixels[9];

        pixels[0] = tmpImage.GetRed(i-1,j-1);
        pixels[1] = tmpImage.GetRed(i-1,j);
        pixels[2] = tmpImage.GetRed(i-1,j+1);
        pixels[3] = tmpImage.GetRed(i,j-1);
        pixels[4] = tmpImage.GetRed(i,j);
        pixels[5] = tmpImage.GetRed(i,j+1);
        pixels[6] = tmpImage.GetRed(i+1,j-1);
        pixels[7] = tmpImage.GetRed(i+1,j);
        pixels[8] = tmpImage.GetRed(i+1,j+1);

        sort(pixels,pixels + sizeof pixels / sizeof pixels[0]);

        loadedImage.SetRGB(i, j, pixels[4], pixels[4], pixels[4]);
     }
  }

  tmpImage.Destroy();

  printf("Finished Applying Max Filter.\n");
  Refresh();
}

void MyFrame::OnNegative(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Negative Filter...");
  wxImage tmpImage = loadedImage.Copy();

  for(int i=0; i < loadedImage.GetWidth(); i++) {
      for(int j=0; j < loadedImage.GetHeight(); j++) {
        int originalPixel = loadedImage.GetRed(i,j);
        int pixel = abs(originalPixel - 255);
        loadedImage.SetRGB(i, j, pixel, pixel, pixel);
     }
  }

  tmpImage.Destroy();

  printf("Finished Negative Filter.\n");
  Refresh();
}

void MyFrame::OnLog(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Log Filter...");
  wxImage tmpImage = loadedImage.Copy();
  for(int i = 0; i < loadedImage.GetWidth(); i++) {
      for(int j = 0; j < loadedImage.GetHeight(); j++) {
        int constant = 70;
        int originalPixel = loadedImage.GetRed(i,j);
        long pixel = constant * log(originalPixel + 1);
        if(pixel > 255) {
          pixel = 255;
        }

        loadedImage.SetRGB(i, j, pixel, pixel, pixel);
     }
  }

  tmpImage.Destroy();

  printf("Finished Log Filter.\n");
  Refresh();
}

void MyFrame::OnPower(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Max Filter...");
  wxImage tmpImage = loadedImage.Copy();

  for(int i = 0; i < loadedImage.GetWidth(); i++) {
      for(int j = 0; j < loadedImage.GetHeight(); j++) {
        double constant = 10;
        double gamma = 0.8;

        int originalPixel = loadedImage.GetRed(i,j);
        long pixel = constant * pow((originalPixel), gamma);
        if(pixel > 255) {
          pixel = 255;
        }

        loadedImage.SetRGB(i, j, pixel, pixel, pixel);
     }
  }

  tmpImage.Destroy();

  printf("Finished Applying Max Filter.\n");
  Refresh();
}

void MyFrame::OnHistogramEqualize(wxCommandEvent & WXUNUSED(event)) {

  printf("Applying Histogram Equalization...");
  wxImage tmpImage = loadedImage.Copy();
  map<int,double> pixelValMap;

  int count = 0;

  for(int i = 0; i < loadedImage.GetWidth(); i++) {
      for(int j = 0; j < loadedImage.GetHeight(); j++) {

        int originalPixel = loadedImage.GetRed(i,j);

        if(pixelValMap.count(originalPixel) > 0) {
          pixelValMap[originalPixel] = pixelValMap[originalPixel] + 1.0;
        } else {
          pixelValMap[originalPixel] = 1.0;
        }

        count++;
     }
  }

  int pixelSize = pixelValMap.size() - 1;
  double cumalative = 0;



  typedef std::map<int, double>::iterator it_type;
  for(it_type iterator = pixelValMap.begin(); iterator != pixelValMap.end(); iterator++) {
      cumalative += iterator->second;
      iterator->second = round((cumalative*pixelSize)/count);
      cout << iterator->first << " | " << iterator->second << endl;
  }

  for(int i = 0; i < loadedImage.GetWidth(); i++) {
      for(int j = 0; j < loadedImage.GetHeight(); j++) {

        int originalPixel = loadedImage.GetRed(i,j);

        if(pixelValMap.count(originalPixel) > 0) {
          unsigned char pixel = pixelValMap[originalPixel];
          loadedImage.SetRGB(i, j, pixel, pixel, pixel);

        }
     }
  }


  tmpImage.Destroy();

  printf("Finished Applying Histogram Equalization.\n");
  Refresh();
}

//IMAGE SAVING
void MyFrame::OnSaveImage(wxCommandEvent & WXUNUSED(event)) {

    printf("Saving image...");

    loadedImage.SaveFile(wxT("Saved_Image.bmp"), wxBITMAP_TYPE_BMP);

    printf("Finished Saving.\n");
}

void MyFrame::OnExit (wxCommandEvent & WXUNUSED(event)) {

    Close(TRUE);
}

void MyFrame::OnPaint(wxPaintEvent & WXUNUSED(event)) {

    wxAutoBufferedPaintDC dc(this);
    if(loadedImage.Ok()) {
        dc.DrawBitmap(wxBitmap(loadedImage), 0, 0, false);//given bitmap xcoord y coord and transparency
    }
}

BEGIN_EVENT_TABLE (MyFrame, wxFrame)
    EVT_MENU ( LOAD_FILE_ID,  MyFrame::OnOpenFile)
    EVT_MENU ( EXIT_ID,  MyFrame::OnExit)

//###########################################################//
//----------------------START MY EVENTS ---------------------//
//###########################################################//

    EVT_MENU ( RESET_IMAGE_ID,  MyFrame::OnResetImage)
    EVT_MENU ( INVERT_IMAGE_ID,  MyFrame::OnInvertImage)
    EVT_MENU ( SCALE_IMAGE_ID,  MyFrame::OnScaleImage)
    EVT_MENU ( SAVE_IMAGE_ID,  MyFrame::OnSaveImage)
    EVT_MENU ( PIXEL_SHIFT_ID,  MyFrame::OnPixelShift)//--->To be modified!
    EVT_MENU ( OPEN_RAW_ID,  MyFrame::OnOpenRawFile)//--->To be modified!
    EVT_MENU ( SAVE_RAW_TO_JPG,  MyFrame::OnConvertRawToJPG)//--->To be modified!
    EVT_MENU ( AVERAGE_FILTER_ID,  MyFrame::OnAverageFilter)//--->To be modified!
    EVT_MENU ( WEIGHTED_AVERAGE_FILTER_ID,  MyFrame::OnWeightedAverageFilter)//--->To be modified!
    EVT_MENU ( FOUR_NEIGHBOUR_FILTER_ID,  MyFrame::On4NeighbourFilter)//--->To be modified!
    EVT_MENU ( EIGHT_NEIGHBOUR_FILTER_ID,  MyFrame::On8NeighbourFilter)//--->To be modified!
    EVT_MENU ( FOUR_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID,  MyFrame::On4NeighbourLaplacianEnhancementFilter)//--->To be modified!
    EVT_MENU ( EIGHT_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID,  MyFrame::On8NeighbourLaplacianEnhancementFilter)//--->To be modified!
    EVT_MENU ( ROBERTS_ID,  MyFrame::OnRobertsFilter)//--->To be modified!
    EVT_MENU ( SOBEL_X,  MyFrame::OnSobelXFilter)//--->To be modified!
    EVT_MENU ( SOBEL_Y,  MyFrame::OnSobelYFilter)//--->To be modified!
    EVT_MENU ( SALT_AND_PEPPER_ID,  MyFrame::OnSaltAndPepper)//--->To be modified!
    EVT_MENU ( MIN_ID,  MyFrame::OnMinFilter)//--->To be modified!
    EVT_MENU ( MAX_ID,  MyFrame::OnMaxFilter)//--->To be modified!
    EVT_MENU ( MIDPOINT_ID,  MyFrame::OnMidpointFilter)//--->To be modified!
    EVT_MENU ( MEDIAN_ID,  MyFrame::OnMedianFilter)//--->To be modified!
    EVT_MENU ( NEGATIVE_ID,  MyFrame::OnNegative)//--->To be modified!
    EVT_MENU ( LOG_ID,  MyFrame::OnLog)//--->To be modified!
    EVT_MENU ( POWER_ID,  MyFrame::OnPower)//--->To be modified!
    EVT_MENU ( HISTOGRAM_EQUALIZE,  MyFrame::OnHistogramEqualize)//--->To be modified!




//###########################################################//
//----------------------END MY EVENTS -----------------------//
//###########################################################//

    EVT_PAINT (MyFrame::OnPaint)
END_EVENT_TABLE()
