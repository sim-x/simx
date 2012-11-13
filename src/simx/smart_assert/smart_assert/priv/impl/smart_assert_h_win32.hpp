// impl/smart_assert_h_win32.hpp

// Boost.SmartAssert library
//
// Copyright (C) 2003 John Torjo (john@torjo.com)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
 
// See http://www.boost.org for updates, documentation, and revision history.

// to know the version of the SMART_ASSERT you're using right now, 
// check out <smart_assert/smart_assert/version.txt>

#ifndef BOOST_INNER_SMART_ASSERT_HANDLERS_WIN32_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HANDLERS_WIN32_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

//
// classes for handling ASSERTs for Win32 - UI
//
// note: #include <simx/smart_assert/smart_assert_ext.hpp> instead !!!


#ifdef BOOST_SMART_ASSERT_WIN32

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ts.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_defs.hpp>

#include <windows.h>
#include <new>
#include <ctype.h>


namespace boost { 

namespace smart_assert {

namespace Private {

    // helper
    HWND create_push_button( int left, int top, int width, int height, const char_type * name, HWND hwndParent) {

        CREATESTRUCT cs;
        ::ZeroMemory(&cs, sizeof(cs));
        cs.hInstance = ::GetModuleHandleA(0);
        cs.x = left;
        cs.y = top;
        cs.cx = width;
        cs.cy = height;
        cs.style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_TEXT; 
        cs.lpszName = name;
        cs.lpszClass = "Button";
        cs.dwExStyle = 0;
        cs.hwndParent = hwndParent;

        HWND hwnd = ::CreateWindowExA(
            cs.dwExStyle, cs.lpszClass, cs.lpszName,
            cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, &cs);

        HGDIOBJ var_font = ::GetStockObject( ANSI_VAR_FONT);
        ::SendMessage( hwnd, WM_SETFONT, (WPARAM)var_font, TRUE);
        return hwnd;    
    }

    // helper
    HWND create_edit( HWND hwndParent) {
        CREATESTRUCT cs;
        ::ZeroMemory(&cs, sizeof(cs));
        cs.hInstance = ::GetModuleHandleA(0);
        RECT rc;
        if (!::GetClientRect( hwndParent, &rc)) 
            return 0;

        cs.x = cs.y = 0;
        cs.cx = rc.right - rc.left;
        cs.cy = rc.bottom - rc.top - 30;
        cs.style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_MULTILINE | 
            ES_AUTOVSCROLL | ES_READONLY |
            WS_HSCROLL | WS_VSCROLL;
        cs.lpszName = 0;
        cs.lpszClass = "RichEdit"; // RichEdit 1.0
        cs.dwExStyle = WS_EX_CLIENTEDGE;
        cs.hwndParent = hwndParent;
        HWND hwnd = ::CreateWindowExA(cs.dwExStyle, cs.lpszClass, cs.lpszName,
            cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, &cs);
        HGDIOBJ var_font = ::GetStockObject( ANSI_VAR_FONT);
        ::SendMessage( hwnd, WM_SETFONT, (WPARAM)var_font, TRUE);
        return hwnd;
    }


    // sets the bool in its constructor, and clears it in its destructor
    struct set_and_clear {
        set_and_clear( bool & b) : m_b( b) {
            m_b = true;
        }
        ~set_and_clear() {
            m_b = false;
        }
        bool m_b;
    };

    // many thanks Pavel Vozenilek!
    show_dlg_result show_assert_dialog(
        int nLevel, const char_type * title, const char_type* text, const char_type * text_detailed) 
    {
        static mutex_type s_cs;
        lock_type locker( s_cs);
        static bool s_already_in = false;
        if ( s_already_in)
            // handle recursion
            return already_shown;

        set_and_clear s( s_already_in);

        if (!text || !text[0]) return show_failed;
        HWND wnd = 0;
        HWND richedit_wnd = 0;
        HANDLE dll = 0;
        RECT rc;
        show_dlg_result nRet = show_failed; 
        HWND wnd_ignore;
        HWND wnd_ignore_forever;
        HWND wnd_ignore_all;
        HWND wnd_debug;
        HWND wnd_abort;
        HWND wnd_more;
        MSG msg;

#ifndef BOOST_NO_EXCEPTIONS
        try {
#endif
        // create class for the dialog
        WNDCLASS wndclass;
        ::ZeroMemory(&wndclass, sizeof(wndclass));

        if ( nLevel <= lvl_warn) {
            // warning
            wndclass.hIcon = ::LoadIconA(0, IDI_WARNING);
            wndclass.lpszClassName = "Boost.SmartAssert library assert dialog warn";
        }
        else if ( nLevel <= lvl_debug) {
            // debug
            wndclass.hIcon = ::LoadIconA(0, IDI_WARNING);
            wndclass.lpszClassName = "Boost.SmartAssert library assert dialog warn";
        }
        else if ( nLevel <= lvl_error) {
            // error
            wndclass.hIcon = ::LoadIconA(0, IDI_ERROR);
            wndclass.lpszClassName = "Boost.SmartAssert library assert dialog err";
        }
        else {
            // fatal
            wndclass.hIcon = ::LoadIconA(0, IDI_ERROR);
            wndclass.lpszClassName = "Boost.SmartAssert library assert dialog err";
        }

        // try not to generate WM_PAINT in the asserted application
        wndclass.style = CS_SAVEBITS | CS_NOCLOSE;                       
        // key presses are intercepted in main loop (ugly but keeps it simple)
        wndclass.lpfnWndProc = &::DefWindowProc; 
        wndclass.hInstance = ::GetModuleHandleA(0);
        if (!wndclass.hInstance) return show_failed;
        wndclass.hCursor = ::LoadCursorA(0, IDC_ARROW);
        if (!wndclass.hCursor) return show_failed;

        if (!wndclass.hIcon) return show_failed;
        wndclass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_MENU + 1); // not important
        if (!::RegisterClassA(&wndclass)) {
            // already exists
        }

        // FIXME (nice to have) - what should I do for WinCE?
        // (where width/height are very small?)
        int screen_width = ::GetSystemMetrics(SM_CXFULLSCREEN);
        int screen_height = ::GetSystemMetrics(SM_CYFULLSCREEN);
        if (screen_width == 0 || screen_height == 0) return show_failed;
        int dialog_width = 500; // pixels
        int dialog_height = 250;
        if (screen_width < dialog_width) {
            dialog_width = screen_width;
        }
        if (screen_height < dialog_height) {
            dialog_height = screen_height;
        }

        CREATESTRUCT cs;
        ::ZeroMemory(&cs, sizeof(cs));
        cs.hInstance = ::GetModuleHandleA(0);
        cs.x = screen_width / 2 - dialog_width / 2;
        cs.y = screen_height / 2 - dialog_height / 2;
        cs.cx = dialog_width;
        cs.cy = dialog_height;
        cs.style = WS_CAPTION | WS_CLIPCHILDREN | WS_OVERLAPPED | 
            WS_VISIBLE | WS_SYSMENU;
        cs.lpszName = title;
        cs.lpszClass = wndclass.lpszClassName;
        cs.dwExStyle = WS_EX_CONTROLPARENT | WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST;

        // create dialog window
        wnd = ::CreateWindowExA(cs.dwExStyle, cs.lpszClass, cs.lpszName, 
            cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, &cs);
        if (wnd == 0) goto done;
        if (!::GetClientRect(wnd, &rc)) goto done;

        // insert RichEdit inside
        dll = LoadLibraryA("Riched32.dll"); // the DLL must be loaded first
        if (!dll) goto done;

        richedit_wnd = create_edit( wnd);
        if (!richedit_wnd) goto done;

        // insert text into RichEdit
        if (::SendMessageA(richedit_wnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text)) != TRUE) goto done;

        wnd_ignore = create_push_button( 10, rc.bottom - 25, 70, 20, "&Ignore", wnd);
        wnd_ignore_forever = create_push_button( 90, rc.bottom - 25, 70, 20, "Ign. &Forever", wnd);
        wnd_ignore_all = create_push_button( 170, rc.bottom - 25, 70, 20, "Ignore &All", wnd);
        wnd_debug = create_push_button( 250, rc.bottom - 25, 70, 20, "&Debug", wnd);
        wnd_abort = create_push_button( 330, rc.bottom - 25, 70, 20, "A&bort", wnd);
        wnd_more = create_push_button( 410, rc.bottom - 25, 70, 20, "&More Details", wnd);
        if ( !wnd_ignore || !wnd_ignore_forever || !wnd_ignore_all ||
                 !wnd_debug || !wnd_abort || !wnd_more) 
             goto done;

        // process messages 
        nRet = do_nothing;
        while (::GetMessage(&msg, 0, 0, 0)) {
            ::TranslateMessage(&msg);

            // find out if the user has pressed chosen an Option
            if ( msg.hwnd == wnd_ignore) {
                if ( msg.message == WM_LBUTTONUP)
                    nRet = ignore_now;
            }
            else if ( msg.hwnd == wnd_ignore_forever) {
                if ( msg.message == WM_LBUTTONUP)
                    nRet = ignore_forever;
            }
            else if ( msg.hwnd == wnd_ignore_all) {
                if ( msg.message == WM_LBUTTONUP)
                    nRet = ignore_all;
            }
            else if ( msg.hwnd == wnd_debug) {
                if ( msg.message == WM_LBUTTONUP)
                    nRet = do_debug;
            }
            else if ( msg.hwnd == wnd_abort) {
                if ( msg.message == WM_LBUTTONUP)
                    nRet = do_abort;
            }
            else if ( msg.hwnd == wnd_more) {
                if ( msg.message == WM_LBUTTONUP)
                    nRet = do_show_more;
            }
            else if ( msg.hwnd == wnd) {
                if ( ( msg.message == WM_CHAR) || ( msg.message == WM_SYSCHAR)) {
                    char_type ch = tolower( msg.wParam);
                    switch ( ch) {
                    case 'i': 
                        nRet = ignore_now; break;
    
                    case 'f':
                        nRet = ignore_forever; break;

                    case 'a':
                        nRet = ignore_all; break;

                    case 'd':
                        nRet = do_debug; break;

                    case 'b':
                        nRet = do_abort; break;

                    case 'm':
                        nRet = do_show_more; break;

                    default:
                        ;
                    }
                }
            }

            // what should we do?
            bool bEndDialog = false;
            switch ( nRet) {
            case do_show_more:
                ::SendMessageA(
                    richedit_wnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text_detailed));
                // falls through

            default:
            case do_nothing:
                ::DispatchMessage(&msg);
                break;

            case ignore_now:
            case ignore_forever:
            case ignore_all:
            case do_debug:
            case do_abort:
                bEndDialog = true;
                break;
            };

            if ( bEndDialog)
                break;
        } // while
#ifndef BOOST_NO_EXCEPTIONS
        } catch(...) {
        }
#endif

    done:
#ifndef BOOST_NO_EXCEPTIONS
        try {
#endif
            if (wnd) ::DestroyWindow(wnd);
            if (dll) ::FreeLibrary(reinterpret_cast<HMODULE>(dll));
#ifndef BOOST_NO_EXCEPTIONS
        } catch(...) {
        }
#endif
        return nRet;
    } // show_assert_dialog()


} // namespace Private




} // namespace smart_assert

} // namespace boost


#endif // BOOST_SMART_ASSERT_WIN32

#endif
