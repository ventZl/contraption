#pragma once

#include <cmrx/rpc/interface.h>
#include <stdint.h>
#include <stdbool.h>
#include "factory.h"

#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       720

#define MAX_WINDOWS 64
#define MAX_GADGETS 256

#define WINDOW_NONE         -1
#define PIXMAP_NONE         -1

struct CGadget;

/* Flags for client connections */
enum ClientFlags {
    CLIENT_DOCK = 1,    // Application is dock, it will receive notifications
    CLIENT_SKIP_DOCK    // Skip the application from dock notifications
};

/* Type of event received from server */
enum GadgetEvent {
    EVENT_NONE = 0,
    EVENT_MENU_CLOSED,  // Menu was closed
    EVENT_KEY_PRESSED,          // Key was pressed
    EVENT_CLIENT_CONNECTED,     // New client has opened connection
    EVENT_CLIENT_DISCONNECTED,  // Existing client has dropped connection
    EVENT_CLIENT_NEW_ICON,      // Client has uploaded new icon
    EVENT_WINDOW_CLOSED         // Window was closed
};

/* Type of background window element has */
enum CWindowBackground {
    BACKGROUND_WINDOW = 0,      // Standard window background
    BACKGROUND_DESKTOP,         // Window is desktop
    BACKGROUND_WHITE,           // Plain white background
    BACKGROUND_BLACK,           // Plain black background
    BACKGROUND_DOCK             // Dock background
};

/* Flags for windows */
enum CWindowFlags {
    WINDOW_FLAG_BORDER = 1,     // Window has border
    WINDOW_FLAG_BELOW_ALL = 2,  // Window is never raised above others
    WINDOW_FLAG_FIX_SIZE = 4,   // Window is not resizable despite it has titlebar
    WINDOW_FLAG_NO_HIDE = 8
};

/* Structure describing window */
struct CWindow {
    int left, top;
    int width, height;
    int min_width, min_height;
    int max_width, max_height;

    char title[64];

    uint32_t background;
    unsigned flags;

    struct CGadget * gadgets;
    unsigned gadget_count;
};

/* Internal properties of window */
struct CWindowInternal {
    struct CWindow properties;
    int menu_window_id;
    int id;
    bool is_menu;
    unsigned owner_thread;
};

#define BUTTON_RADIUS 4

/* Known types of gadgets */
enum CGadgetType {
    GADGET_BUTTON = 0,          // pushbutton
    GADGET_TEXT,                // passive text
    GADGET_TITLE_BAR,           // title bar
    GADGET_RADIO,               // radio button, one of many can be selected
    GADGET_CHECKBOX,            // checkbox
    GADGET_TEXT_INPUT,          // one-line inputbox
    GADGET_MENU_ITEM,           // menu item in menu bar or drop-down menu
    GADGET_PANEL,               // passive panel rendering window background
    GADGET_PIXMAP,              // image
    GADGET_TYPE_HIDDEN,         // this gadget type is not mapped to anything. changing gadget type to this will effectively hide it
    GADGET_KNOWN_TYPES          // total amount of known gadget types
};

/* Flags for gadgets */
enum CGadgetFlags {
    GADGET_FLAG_ENABLED = 1,    // gadget is enabled
    GADGET_FLAG_FOCUSABLE = 2,  // gadget is focusable
    GADGET_FLAG_ACTIVABLE = 4,  // gadget can be activated, if it supports activation
    GADGET_FLAG_CLICKABLE = 8   // gadget accepts clicks
};

/* State of gadget */
enum CGadgetState {
    GADGET_STATE_FOCUSED = 1,   // gadget is focused
    GADGET_STATE_HOVERED = 2,   // mouse is hovering over gadget
    GADGET_STATE_ACTIVATED = 4, // gadget is activated
    GADGET_STATE_CLICKED = 8    // gadget has received click
};

/* Text properties */
enum CGadgetTextFlags {
    TEXT_ALIGN_LEFT = 1,        // align text to left within gadget extents
    TEXT_ALIGN_RIGHT = 2,       // align text to right within gadget extents
    TEXT_ALIGN_CENTER = 4,      // center text horizontally within gadget extents
    TEXT_ALIGN_TOP = 8,         // vertically align text to top within gadget extents
    TEXT_ALIGN_MIDDLE = 16,     // center text vertically within gadget extents
    TEXT_ALIGN_BOTTOM = 32,     // vertically align text to bottom within gadget extents
    TEXT_BOLD = 64,             // make text bold
    TEXT_ITALIC = 128,          // make text italic
    TEXT_MONO = 256,            // make text monospace
    TEXT_SIZE_32 = 512,         // make text 32px in size
    TEXT_SIZE_48 = 1024,        // make text 48px in size
    TEXT_SIZE_64 = 2048,        // make text 64px in size
};

/* Structure describing gadget */
struct CGadget {
    int left, top;              // coordinates of upper left corner within window
    int width, height;          // dimensions of gadget

    enum CGadgetType type;      // gadget type

    uint32_t fg_color;          // foreground color (RGB)
    // TODO: Do proper memory management here
    char text[80];              // text this gadget contains (the way how it is displayed depends on gadget type)
    uint32_t text_flags;        // text visual flags
    int sub_menu_id;            // ID of submenu attached to this gadget (only applicable to menu item gadget)
    unsigned event_id;          // ID passed to client for events generated from this gadget

    uint32_t state;             // gadget state
    uint32_t flags;             // gadget flags

    uint8_t text_margin_horiz;  // text margin from gadget extents horizontal
    uint8_t text_margin_vert;   // text margin from gadget extents vertical

    int pixmap_id;              // ID of pixmap displayed by this gadget (only applicable to pixmap gadget)
};

/* Internal structure describing gadget */
struct CGadgetInternal {
    struct CGadget properties;
    int window_id;              // internal window ID assigned to window
};

/* Pixmap properties */
struct CPixmap {
    uint16_t width;             // width in pixels
    uint16_t height;            // height in pixels
};

struct CExtent {
    unsigned top, left, right, bottom;
};

struct CPosition {
    long top, left;
};

/* RPC API of contraption server */
struct ContraptionVTable {
    int (*open_window)(INSTANCE(this), const struct CWindow * window);
    void (*close_window)(INSTANCE(this), int window);
    void (*raise_window)(INSTANCE(this), int window);
    bool (*load_window)(INSTANCE(this), int window, const struct CGadget * gadgets, unsigned gadget_count);
    bool (*move_window)(INSTANCE(this), int window, unsigned left, unsigned top);
    bool (*resize_window)(INSTANCE(this), int window, unsigned width, unsigned height);
    int (*open_menu)(INSTANCE(this), const struct CWindow * menu);
    void (*close_menu)(INSTANCE(this), int menu);
    void (*attach_menu)(INSTANCE(this), int window, int menu);

    int (*open_connection)(INSTANCE(this), const void * notification_object, const char * name, unsigned flags);
    void (*close_connection)(INSTANCE(this));
    void (*set_connection_icon)(INSTANCE(this), int pixmap_id);
    bool (*get_event)(INSTANCE(this), unsigned * event_id, unsigned * object_id);

    bool (*read_gadget)(INSTANCE(this), int window, unsigned gadget_id, struct CGadget * out);
    bool (*write_gadget)(INSTANCE(this), int window, unsigned gadget_id, const struct CGadget * in);

    int (*open_pixmap)(INSTANCE(this), const struct CPixmap * pixmap);
    int (*load_pixmap)(INSTANCE(this), int pixmap_id, const uint32_t * data);
    int (*close_pixmap)(INSTANCE(this), int pixmap_id);
};

/* Internal status of contraption server */
struct Contraption {
    const struct ContraptionVTable * vtable;
    bool render;
    bool pointer;
    bool button;
    struct CWindowInternal * cursor_window;
    int menu_window_id;
    struct CGadgetInternal * cursor_gadget;
    struct CGadgetInternal * focus_gadget;
    bool cursor_gadget_grab;
    struct CWindowInternal * popup_window;
    struct CPosition last_pos;
    struct CWindowInternal * menu_opener_window;
    struct CGadgetInternal * menu_opener_gadget;
    bool keypress;
};

/* Instance */
extern struct Contraption display;
