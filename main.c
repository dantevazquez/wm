#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WINDOWS 9
#define LEMONBAR_HEIGHT 24
#define MOD Mod4Mask  // Super key

typedef struct {
    Window win;
    int active;
} Client;

Display *dpy;
Window root;
Client clients[MAX_WINDOWS];
int current_client = -1;
int screen_width, screen_height;
Atom net_wm_window_type, net_wm_window_type_dock;

// Generic error handler to prevent crashes on BadWindow, etc.
int x_error_handler(Display *d, XErrorEvent *e) {
    return 0;
}

void update_bar() {
    printf("%%{l}"); 
    int count = 0;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (clients[i].active) {
            count++;
            if (i == current_client) {
                // Active window: Highlight
                printf(" %%{F#ffffff}%%{B#555555} [%d] %%{B-}%%{F-} ", i + 1);
            } else {
                // Inactive window
                printf(" %%{F#aaaaaa} %d %%{F-} ", i + 1);
            }
        }
    }
    if (count == 0) {
        printf(" No windows ");
    }
    printf("\n");
    fflush(stdout);
}

void setup() {
    XSetErrorHandler(x_error_handler); 
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    
    root = DefaultRootWindow(dpy);
    screen_width = DisplayWidth(dpy, DefaultScreen(dpy));
    screen_height = DisplayHeight(dpy, DefaultScreen(dpy));

    net_wm_window_type = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    net_wm_window_type_dock = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    
    // Initialize clients
    for (int i = 0; i < MAX_WINDOWS; i++) {
        clients[i].win = None;
        clients[i].active = 0;
    }
    
    // Select events
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
    
    // Grab keys (Super + 1-9 and Super + Q)
    for (int i = 0; i < 9; i++) {
        XGrabKey(dpy, XKeysymToKeycode(dpy, XK_1 + i), MOD, root, True, GrabModeAsync, GrabModeAsync);
    }
    XGrabKey(dpy, XKeysymToKeycode(dpy, XK_q), MOD, root, True, GrabModeAsync, GrabModeAsync);
    
    XSync(dpy, False);
    update_bar();
}

int add_client(Window w) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!clients[i].active) {
            clients[i].win = w;
            clients[i].active = 1;
            return i;
        }
    }
    return -1;
}

void focus_client(int idx) {
    if (idx < 0 || idx >= MAX_WINDOWS || !clients[idx].active) return;
    
    // Hide current window (only if it's different and valid)
    if (current_client >= 0 && current_client < MAX_WINDOWS && clients[current_client].active && current_client != idx) {
        XUnmapWindow(dpy, clients[current_client].win);
    }
    
    // Show and focus new window
    current_client = idx;
    XMapWindow(dpy, clients[idx].win);
    XRaiseWindow(dpy, clients[idx].win);
    XSetInputFocus(dpy, clients[idx].win, RevertToPointerRoot, CurrentTime);
    update_bar();
}

void remove_client(int idx) {
    if (idx < 0 || idx >= MAX_WINDOWS || !clients[idx].active) return;
    
    // Shift clients down
    for (int i = idx; i < MAX_WINDOWS - 1; i++) {
        clients[i] = clients[i + 1];
    }
    // Clear the last slot
    clients[MAX_WINDOWS - 1].win = None;
    clients[MAX_WINDOWS - 1].active = 0;
    
    // Adjust current_client
    if (current_client == idx) {
        // Focused window was removed. Try to focus the same index (next window shifted in)
        // or the previous one if that was the last window.
        if (clients[idx].active) {
            focus_client(idx);
        } else if (idx > 0 && clients[idx - 1].active) {
            focus_client(idx - 1);
        } else {
            current_client = -1;
            update_bar();
        }
    } else if (current_client > idx) {
        // Focused window shifted down
        current_client--;
        update_bar();
    } else {
        // Focused window was before the removed one, just update bar
        update_bar();
    }
}

void manage_window(Window w) {
    int idx = add_client(w);
    if (idx == -1) {
        fprintf(stderr, "Maximum windows reached\n");
        return;
    }
    
    // Set window geometry (fullscreen minus lemonbar)
    XMoveResizeWindow(dpy, w, 0, LEMONBAR_HEIGHT, screen_width, screen_height - LEMONBAR_HEIGHT);
    
    // Set border
    XSetWindowBorderWidth(dpy, w, 0);
    
    // Map and focus
    focus_client(idx);
}

int is_dock(Window w) {
    // Check _NET_WM_WINDOW_TYPE
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop = NULL;

    if (XGetWindowProperty(dpy, w, net_wm_window_type, 0, sizeof(Atom), False, XA_ATOM,
                           &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success && prop) {
        Atom type = *(Atom *)prop;
        XFree(prop);
        if (type == net_wm_window_type_dock) return 1;
    }

    // Check class hint
    XClassHint ch;
    int result = 0;
    if (XGetClassHint(dpy, w, &ch)) {
        if (ch.res_name && (strcmp(ch.res_name, "lemonbar") == 0 || strcmp(ch.res_name, "bar") == 0)) {
            result = 1;
        }
        if (ch.res_name) XFree(ch.res_name);
        if (ch.res_class) XFree(ch.res_class);
    }
    return result;
}

void handle_map_request(XMapRequestEvent *e) {
    XWindowAttributes attr;
    XGetWindowAttributes(dpy, e->window, &attr);
    if (attr.override_redirect) return;

    if (is_dock(e->window)) {
        XMapWindow(dpy, e->window);
        return;
    }

    manage_window(e->window);
}

void handle_destroy_notify(XDestroyWindowEvent *e) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (clients[i].active && clients[i].win == e->window) {
            remove_client(i);
            break;
        }
    }
}

void handle_key_press(XKeyEvent *e) {
    KeySym key = XLookupKeysym(e, 0);
    
    // Super + 1-9: Switch windows
    if (key >= XK_1 && key <= XK_9) {
        int idx = key - XK_1;
        focus_client(idx);
    }
    // Super + Q: Close current window
    else if (key == XK_q) {
        if (current_client >= 0 && clients[current_client].active) {
            XKillClient(dpy, clients[current_client].win);
        }
    }
}

int main() {
    setup();
    
    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        
        switch (ev.type) {
            case MapRequest:
                handle_map_request(&ev.xmaprequest);
                break;
            case DestroyNotify:
                handle_destroy_notify(&ev.xdestroywindow);
                break;
            case KeyPress:
                handle_key_press(&ev.xkey);
                break;
        }
    }
    
    XCloseDisplay(dpy);
    return 0;
}
