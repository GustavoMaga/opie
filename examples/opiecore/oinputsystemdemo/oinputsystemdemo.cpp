#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>

#include <sys/types.h>
#include <linux/input.h>

using std::string;

const unsigned char     BUT1 = 0x01;
const unsigned char     BUT2 = 0x04;
const unsigned char     BUT3 = 0x02;
const unsigned char     BUT4 = 0x40;
const unsigned char     BUT5 = 0x80;

#define     BITMASK( name, numbits )                                        \
    unsigned short  name[ ((numbits) - 1) / (sizeof( short ) * 8) + 1 ];    \
    memset( name, 0, sizeof( name ) )

#define     TEST_BIT( bitmask, bit )    \
    ( bitmask[ (bit) / sizeof(short) / 8 ] & (1u << ( (bit) % (sizeof(short) * 8))) )

int Open_cPad()
{
    size_t          MAX_DEVICES = 1;
    int             fd = -1;
    input_id        info;

    for( size_t i = 0; i < MAX_DEVICES; ++i )
    {
        string      devpath( "/dev/input/event0" );

        //devpath << (int)i;

        fd = open( devpath.c_str(), O_RDONLY, &info );

        if( fd >= 0 )
        {
            int version = -1;
            /* ioctl() accesses the underlying driver */
            if (ioctl(fd, EVIOCGVERSION, &version)) {
                perror("evdev ioctl");
            }

            /* the EVIOCGVERSION ioctl() returns an int */
            /* so we unpack it and display it */
            printf("evdev driver version is %d.%d.%d\n",
                version >> 16, (version >> 8) & 0xff,
                version & 0xff);

            // Get Identifying info

            if( ioctl( fd, EVIOCGID, &info) )
            {
                perror( "event device ioctl" );
                return -1;
            }
            printf( "Bus: %#x,  Vendor: %#x, Product: %#x, Version: %#x\n",
                    info.bustype, info.vendor, info.product, info.version );

            switch ( info.bustype)
            {
                case BUS_PCI :
                    printf(" is on a PCI bus\n");
                    break;
                case BUS_USB :
                    printf(" is on a Universal Serial Bus\n");
                    break;
                /* ... */
            }

            // Get device 'name'

            char    name[256] = "Unknown";
            if( ioctl( fd, EVIOCGNAME(sizeof name ), name) < 0 )
            {
                perror( "event device ioctl" );
            }else
            printf( "Device name '%s'\n", name );

            if(ioctl(fd, EVIOCGPHYS(sizeof(name)), name) < 0) {
                perror("event ioctl");
            }else
            printf("Device path '%s'\n", name );

            if(ioctl(fd, EVIOCGUNIQ(sizeof(name)), name) < 0) {
                perror("event ioctl");
            }else
            printf("Device identity '%s'\n", name );

            // Get feature types

            BITMASK( features, EV_MAX );

            if( ioctl( fd, EVIOCGBIT( 0, EV_MAX ), features) < 0 )
            {
                perror( "event device ioctl" );
                return -1;
            }

            printf( "Supported features:\n" );
            for( size_t bit = 0; bit < EV_MAX; ++bit )
            {
                if( TEST_BIT( features, bit ) )
                {
                    switch( bit )
                    {
                        case EV_SYN :
                            printf("  Sync. Events\n");
                            break;
                        //case EV_RST:
                        //    printf( "  Reset?\n" );
                        //    break;
                        case EV_KEY:
                            printf( "  Keys or buttons\n" );
                            break;
                        case EV_REL:
                            printf( "  Relative axes\n" );
                            break;
                        case EV_ABS:
                            printf( "  Absolute  axes\n" );
                            break;
                        case EV_MSC:
                            printf( "  Misc\n" );
                            break;
                        case EV_LED:
                            printf( "  Led's\n" );
                            break;
                        case EV_SND:
                            printf( "  Sounds\n" );
                            break;
                        case EV_REP:
                            printf( "  Repeat\n" );
                            break;
                        case EV_FF : // fallthrough
                        case EV_FF_STATUS:
                            printf("   Force Feedback\n");
                            break;
                        case EV_PWR:
                            printf("   Power Management\n");
                            break;
                       default:
                            printf( " Unknown (bit %d)\n", bit );
                    }
                }
            }

            // Check keystate

            BITMASK( keys, KEY_MAX );

            if( ioctl( fd, EVIOCGKEY( sizeof(keys) ), keys ) < 0 )
            {
                perror( "event device ioctl" );
                return -1;
            }

            printf( "Key global status:\n" );
            for( size_t bit = 0; bit < KEY_MAX; ++bit )
            {
                if( TEST_BIT( keys, bit ) )
                {
                    printf( "Key (bit %d) active\n", bit );
                }
            }
        }
        else
            printf( "Failed to open device\n" );

        close( fd );
    }
    return fd;
}


int main( int argc, char **argv )
{
    //printf(" Long: %d, int %d, short %d\n", sizeof( long ), sizeof( int ), sizeof(  short ) );
    //printf( "num chars %d\n", snprintf( NULL, 0, 0 ) );
    //exit( 0 );
    Open_cPad();

#if 1
    int     fd = open( "/dev/input/event0", O_RDONLY );

    if( fd == -1 )
    {
        printf( "Failed to open device\n" );
        goto hell;
    }

    struct input_event  evbuf;

    for(;;)
    {
        if( read( fd, &evbuf, sizeof evbuf ) == -1 )
        {
            printf( "Read error\n" );
            goto hell;
        }

        printf( "Type: %d, Code: %d, Value: %d\n", evbuf.type, evbuf.code, evbuf.value );
    }

#endif
#if 0
    unsigned char   buf[ 4 ];

    for(;;)
    {
        if( read( fd, buf, sizeof buf ) == -1 )
        {
            printf( "Read error\n" );
            goto hell;
        }

        printf( "Raw:\t%#x\t%#x\t%#x\t%#x\n", buf[ 0 ], buf[ 1 ], buf[ 2 ], buf[ 3 ] );

        int     dx = buf[ 1 ];

        if( ( buf[ 0 ] & 0x10 ) != 0 )
            dx -= 256;

        int     dy = - buf[ 2 ];

        if( ( buf[ 0 ] & 0x20 ) != 0 )
            dy += 256;

        printf( "( %d, %d )\t", dx, dy );

        if( buf[ 0 ] & BUT1 )
            printf( "Left\t" );
        if( buf[ 0 ] & BUT2 )
            printf( "Middle\t" );
        if( buf[ 0 ] & BUT3 )
            printf( "Right\t" );

        printf( "\n" );
    }
#endif

    //close( fd );

    exit( EXIT_SUCCESS );
hell:
    exit( EXIT_FAILURE );
}

