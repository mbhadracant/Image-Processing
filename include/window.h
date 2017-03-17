using namespace std;

class BasicApplication : public wxApp {
    public:
        virtual bool OnInit();
};

class MyFrame : public wxFrame {
    protected:
        wxMenuBar  *menuBar;//main menu bar
        wxMenu     *fileMenu;//file menu
        wxMenu     *editMenu;//file menu
        wxMenu     *lab4Menu;//file menu
        wxMenu     *lab5Menu;//file menu
        wxMenu     *lab6Menu;//file menu
        wxMenu     *lab7Menu;//file menu
        wxMenu     *lab8Menu;//file menu
        wxMenu     *lab9Menu;//file menu
        wxToolBar  *toolbar;//tollbar not necessary to use

        bool canSelect, selected;
        int oldWidth, oldHeight; // save old dimensions
        int mouseDownX, mouseDownY, mouseUpX, mouseUpY;
        wxImage loadedImage, origImage, tmpImage; // image loaded from file and original placeholder
        stack<wxImage> undoStack;
        stack<wxImage> origImageStack;

        /* declear message handler */
        void OnResetImage(wxCommandEvent & event);
        void OnUndo(wxCommandEvent & event);
        void OnScaleImage(wxCommandEvent & event);
        void OnSaveImage(wxCommandEvent & event);
        void OnPixelShift(wxCommandEvent & event);
        void OnOpenRawFile(wxCommandEvent & event);
        void OnConvertRawToJPG(wxCommandEvent & event);
        void OnAverageFilter(wxCommandEvent & event);
        void OnWeightedAverageFilter(wxCommandEvent & event);
        void On4NeighbourFilter(wxCommandEvent & event);
        void On8NeighbourFilter(wxCommandEvent & event);
        void On4NeighbourLaplacianEnhancementFilter(wxCommandEvent & event);
        void On8NeighbourLaplacianEnhancementFilter(wxCommandEvent & event);
        void OnRobertsFilter(wxCommandEvent & event);
        void OnSobelXFilter(wxCommandEvent & event);
        void OnSobelYFilter(wxCommandEvent & event);
        void OnSaltAndPepper(wxCommandEvent & event);
        void OnMinFilter(wxCommandEvent & event);
        void OnMaxFilter(wxCommandEvent & event);
        void OnMidpointFilter(wxCommandEvent & event);
        void OnMedianFilter(wxCommandEvent & event);
        void OnNegative(wxCommandEvent & event);
        void OnPower(wxCommandEvent & event);
        void OnLog(wxCommandEvent & event);
        void OnRandomLookupTable(wxCommandEvent & event);
        void OnHistogramEqualize(wxCommandEvent & event);
        void OnMeanAndStandardDeviation(wxCommandEvent & event);
        void OnSimpleThresholding(wxCommandEvent & event);
        void OnAutomatedThresholding(wxCommandEvent & event);
        void OnMouseDown(wxMouseEvent& event);
        void OnMouseUp(wxMouseEvent& event);
        void OnSelect(wxCommandEvent & event);
        void OnUnSelect(wxCommandEvent & event);
        void ResetDimensionsToOriginal();
        void UseOriginalImageIfNoTemp();
        double getMeanOfHistogramRange(map<unsigned char, unsigned char> pixelValueMap, double start, double end);
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
    UNDO,
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
    RANDOM_LOOKUP_TABLE,
    HISTOGRAM_EQUALIZE,
    MEAN_AND_STANDARD_DEVIATION,
    SIMPLE_THRESHOLDING,
    AUTOMATED_THRESHOLDING,
    SELECT,
    UNSELECT
};
