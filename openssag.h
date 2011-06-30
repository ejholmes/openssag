#ifndef __OPEN_SSAG_H__
#define __OPEN_SSAG_H__ 

#define VENDOR_ID 0x1856 /* Orion Telescopes VID */
#define PRODUCT_ID 0x0012 /* SSAG IO PID */

typedef struct usb_dev_handle usb_dev_handle;

typedef struct raw_image {
    int width;
    int height;
    unsigned char *data;
} raw_image;

/* Guide Directions (cardinal directions) */
enum guide_direction {
    guide_east = 0x10,
    guide_south = 0x20,
    guide_north = 0x40,
    guide_west = 0x80,
};

namespace OpenSSAG
{
    class SSAG
    {
    private:
        /* Holds the converted gain */
        unsigned short gain;

        /* Handle to the device */
        usb_dev_handle *handle;

        /* Sends init packet and pre expose request */
        void InitSequence();

        /* Gets the data from the autoguiders internal buffer */
        unsigned char *ReadBuffer();
    public:
        /* Connect to the autoguider */
        bool Connect();

        /* Disconnect from the autoguider */
        void Disconnect();

        /* Gain should be a value between 1 and 15 */
        void SetGain(int gain);

        /* Expose and return the image in raw gray format. Function is blocking. */
        raw_image *Expose(int duration);

        /* Issue a guide command through the guider relays */
        void Guide(enum guide_direction direction, int duration);

        /* Frees a raw_image struct */
        void FreeRawImage(raw_image *image);
    };
}

#endif /* __OPEN_SSAG_H__ */
