
class BasicApplication : public wxApp {
    public:
        virtual bool OnInit();
};

class MyFrame : public wxFrame {
    protected:
        wxMenuBar  *menuBar;//main menu bar
        wxMenu     *fileMenu;//file menu
        wxMenu     *lab4Menu;//file menu
        wxMenu     *lab5Menu;//file menu
        wxMenu     *lab6Menu;//file menu
        wxMenu     *lab7Menu;//file menu
        wxMenu     *lab8Menu;//file menu
        wxMenu     *lab9Menu;//file menu
        wxToolBar  *toolbar;//tollbar not necessary to use

        int oldWidth, oldHeight; // save old dimensions

        wxImage loadedImage, origImage; // image loaded from file and original placeholder

        /* declear message handler */
        void OnResetImage(wxCommandEvent & event);
        void OnInvertImage(wxCommandEvent & event);
        void OnScaleImage(wxCommandEvent & event);
        void OnSaveImage(wxCommandEvent & event);
        void OnPixelShift(wxCommandEvent & event); //---> To be modified!
        void OnOpenRawFile(wxCommandEvent & event); //---> To be modified!
        void OnConvertRawToJPG(wxCommandEvent & event); //---> To be modified!
        void OnAverageFilter(wxCommandEvent & event); //---> To be modified!
        void OnWeightedAverageFilter(wxCommandEvent & event); //---> To be modified!
        void On4NeighbourFilter(wxCommandEvent & event); //---> To be modified!
        void On8NeighbourFilter(wxCommandEvent & event); //---> To be modified!
        void On4NeighbourLaplacianEnhancementFilter(wxCommandEvent & event); //---> To be modified!
        void On8NeighbourLaplacianEnhancementFilter(wxCommandEvent & event); //---> To be modified!
        void OnRobertsFilter(wxCommandEvent & event); //---> To be modified!
        void OnSobelXFilter(wxCommandEvent & event); //---> To be modified!
        void OnSobelYFilter(wxCommandEvent & event); //---> To be modified!
        void OnSaltAndPepper(wxCommandEvent & event); //---> To be modified!
        void OnMinFilter(wxCommandEvent & event); //---> To be modified!
        void OnMaxFilter(wxCommandEvent & event); //---> To be modified!
        void OnMidpointFilter(wxCommandEvent & event); //---> To be modified!
        void OnMedianFilter(wxCommandEvent & event); //---> To be modified!
        void OnNegative(wxCommandEvent & event); //---> To be modified!
        void OnPower(wxCommandEvent & event); //---> To be modified!
        void OnLog(wxCommandEvent & event); //---> To be modified!
        void OnHistogramEqualize(wxCommandEvent & event); //---> To be modified!


    public:
        MyFrame(const wxString title, int xpos, int ypos, int width, int height);
        virtual ~MyFrame();

        void OnExit(wxCommandEvent & event);
        void OnOpenFile(wxCommandEvent & event);
        void OnPaint(wxPaintEvent & event);

        DECLARE_EVENT_TABLE()
};

enum {
    EXIT_ID = wxID_HIGHEST + 1,
    LOAD_FILE_ID,
    RESET_IMAGE_ID,
    INVERT_IMAGE_ID,
    SCALE_IMAGE_ID,
    SAVE_IMAGE_ID,
    MY_IMAGE_ID, //--->To be modified!
    OPEN_RAW_ID,
    SAVE_RAW_TO_JPG,
    PIXEL_SHIFT_ID,
    AVERAGE_FILTER_ID,
    WEIGHTED_AVERAGE_FILTER_ID,
    FOUR_NEIGHBOUR_FILTER_ID,
    FOUR_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID,
    EIGHT_NEIGHBOUR_FILTER_ID,
    EIGHT_NEIGHBOUR_LAPLACIAN_ENHANCEMENT_FILTER_ID,
    ROBERTS_ID,
    SOBEL_X,
    SOBEL_Y,
    SALT_AND_PEPPER_ID,
    MIN_ID,
    MAX_ID,
    MIDPOINT_ID,
    MEDIAN_ID,
    NEGATIVE_ID,
    LOG_ID,
    POWER_ID,
    HISTOGRAM_EQUALIZE
};
