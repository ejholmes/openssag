#ifndef __OPEN_SSAG_H__ 
#define __OPEN_SSAG_H__ 

#define VENDOR_ID 0x1856 /* Orion Telescopes VID */
#define PRODUCT_ID 0x0012 /* SSAG IO PID */

#define LOADER_VENDOR_ID 0x1856 /* Orion Telescopes VID */
#define LOADER_PRODUCT_ID 0x0011 /* Loader PID for loading firmware */

typedef struct usb_dev_handle usb_dev_handle;

namespace OpenSSAG
{
    /* Struct used to return image data */
    struct raw_image {
        unsigned int width;
        unsigned int height;
        unsigned char *data;
    };

    /* Guide Directions (cardinal directions) */
    enum guide_direction {
        guide_east  = 0x10,
        guide_south = 0x20,
        guide_north = 0x40,
        guide_west  = 0x80,
    };

    class SSAG
    {
    private:
        /* Sends init packet and pre expose request */
        void InitSequence();

        /* Gets the data from the autoguiders internal buffer */
        unsigned char *ReadBuffer();

        /* Holds the converted gain */
        unsigned int gain;

        /* Handle to the device */
        usb_dev_handle *handle;
    public:
        /* Constructor */
        SSAG();

        /* Connect to the autoguider. If bootload is set to true and the camera
         * cannot be found, it will attempt to connect to the base device and
         * load the firmware. Defaults to false. */
        bool Connect(bool bootload);
        bool Connect();

        /* Disconnect from the autoguider */
        void Disconnect();

        /* Gain should be a value between 1 and 15 */
        void SetGain(int gain);

        /* Expose and return the image in raw gray format. Function is blocking. */
        struct raw_image *Expose(int duration);

        /* Cancels an exposure */
        void CancelExposure();

        /* Issue a guide command through the guider relays. Guide directions
         * can be OR'd together to move in X and Y at the same time.
         *
         * EX. Guide(guide_north | guide_west, 100, 200); */
        void Guide(enum guide_direction direction, int yduration, int xduration);
        void Guide(enum guide_direction direction, int duration);

        /* Frees a raw_image struct */
        void FreeRawImage(raw_image *image);

    };

    /* This class is used for loading the firmware onto the device after it is
     * plugged in.
     *
     * See Cypress EZUSB fx2 datasheet for more information
     * http://www.keil.com/dd/docs/datashts/cypress/fx2_trm.pdf */
    class Loader
    {
    private:
        /* Puts the device into reset mode by writing 0x01 to CPUCS */
        void EnterResetMode();

        /* Makes the device exit reset mode by writing 0x00 to CPUCS */
        void ExitResetMode();

        /* Handle to the cypress device */
        usb_dev_handle *handle;
    public:
        /* Connects to SSAG Base */
        bool Connect();

        /* Disconnects from SSAG Base */
        void Disconnect();

        /* Loads the firmware into SSAG's RAM */
        void LoadFirmware();
    };
}
 
#endif /* __OPEN_SSAG_H__ */ 
