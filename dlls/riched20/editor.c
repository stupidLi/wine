/*
 * RichEdit - functions dealing with editor object
 *
 * Copyright 2004 by Krzysztof Foltman
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* 
  API implementation status:
  
  Messages (ANSI versions not done yet)
  - EM_AUTOURLDETECT 2.0
  - EM_CANPASTE
  + EM_CANREDO 2.0
  + EM_CANUNDO
  - EM_CHARFROMPOS
  - EM_DISPLAYBAND
  + EM_EMPTYUNDOBUFFER
  + EM_EXGETSEL
  - EM_EXLIMITTEXT
  - EM_EXLINEFROMCHAR
  + EM_EXSETSEL
  - EM_FINDTEXT
  - EM_FINDTEXTEX
  - EM_FINDWORDBREAK
  - EM_FMTLINES
  - EM_FORMATRANGE
  - EM_GETCHARFORMAT (partly done)
  + EM_GETEVENTMASK
  - EM_GETFIRSTVISIBLELINE
  - EM_GETIMECOLOR 1.0asian
  - EM_GETIMECOMPMODE 2.0
  - EM_GETIMEOPTIONS 1.0asian
  - EM_GETIMESTATUS
  - EM_GETLANGOPTIONS 2.0
  - EM_GETLIMITTEXT
  - EM_GETLINE        
  - EM_GETLINECOUNT   returns number of rows, not of paragraphs
  + EM_GETMODIFY
  - EM_GETOLEINTERFACE
  - EM_GETOPTIONS
  + EM_GETPARAFORMAT
  - EM_GETPUNCTUATION 1.0asian
  - EM_GETRECT
  - EM_GETREDONAME 2.0
  + EM_GETSEL
  + EM_GETSELTEXT (ANSI&Unicode)
! - EM_GETTHUMB
  - EM_GETTEXTMODE 2.0
? + EM_GETTEXTRANGE (ANSI&Unicode)
  - EM_GETUNDONAME
  - EM_GETWORDBREAKPROC
  - EM_GETWORDBREAKPROCEX
  - EM_GETWORDWRAPMODE 1.0asian
  - EM_HIDESELECTION
  - EM_LIMITTEXT
  - EM_LINEFROMCHAR
  - EM_LINEINDEX
  - EM_LINELENGTH
  - EM_LINESCROLL
  - EM_PASTESPECIAL
  - EM_POSFROMCHARS
  - EM_REDO 2.0
  - EM_REQUESTRESIZE
  + EM_REPLACESEL (proper style?) ANSI&Unicode
  - EM_SCROLL
  - EM_SCROLLCARET
  - EM_SELECTIONTYPE
  + EM_SETBKGNDCOLOR
  - EM_SETCHARFORMAT (partly done, no ANSI)
  + EM_SETEVENTMASK (few notifications supported)
  - EM_SETIMECOLOR 1.0asian
  - EM_SETIMEOPTIONS 1.0asian
  - EM_SETLANGOPTIONS 2.0
  - EM_SETLIMITTEXT
  + EM_SETMODIFY (not sure if implementation is correct)
  - EM_SETOLECALLBACK
  - EM_SETOPTIONS
  + EM_SETPARAFORMAT
  - EM_SETPUNCTUATION 1.0asian
  + EM_SETREADONLY no beep on modification attempt
  - EM_SETRECT
  - EM_SETRECTNP (EM_SETRECT without repainting) - not supported in RICHEDIT
  + EM_SETSEL
  - EM_SETTARGETDEVICE
  - EM_SETTEXTMODE 2.0
  - EM_SETUNDOLIMIT 2.0
  - EM_SETWORDBREAKPROC
  - EM_SETWORDBREAKPROCEX
  - EM_SETWORDWRAPMODE 1.0asian
  - EM_STOPGROUPTYPING 2.0
  - EM_STREAMIN
  - EM_STREAMOUT
  - EM_UNDO
  + WM_CHAR
  + WM_CLEAR
  - WM_COPY (lame implementation, no RTF support)
  - WM_CUT (lame implementation, no RTF support)
  + WM_GETDLGCODE (the current implementation is incomplete)
  + WM_GETTEXT (ANSI&Unicode)
  + WM_GETTEXTLENGTH (ANSI version sucks)
  - WM_PASTE
  - WM_SETFONT
  + WM_SETTEXT (resets undo stack !) (proper style?) ANSI&Unicode
  - WM_STYLECHANGING
  - WM_STYLECHANGED (things like read-only flag)
  - WM_UNICHAR
  
  Notifications
  
  * EN_CHANGE (sent from the wrong place)
  - EN_CORRECTTEXT
  - EN_DROPFILES
  - EN_ERRSPACE
  - EN_HSCROLL
  - EN_IMECHANGE
  + EN_KILLFOCUS
  - EN_LINK
  - EN_MAXTEXT
  - EN_MSGFILTER
  - EN_OLEOPFAILED
  - EN_PROTECTED
  - EN_REQUESTRESIZE
  - EN_SAVECLIPBOARD
  + EN_SELCHANGE 
  + EN_SETFOCUS
  - EN_STOPNOUNDO
  * EN_UPDATE (sent from the wrong place)
  - EN_VSCROLL
  
  Styles
  
  - ES_AUTOHSCROLL
  - ES_AUTOVSCROLL
  - ES_CENTER
  - ES_DISABLENOSCROLL (scrollbar is always visible)
  - ES_EX_NOCALLOLEINIT
  - ES_LEFT
  - ES_MULTILINE (currently single line controls aren't supported)
  - ES_NOIME
  - ES_READONLY (I'm not sure if beeping is the proper behaviour)
  - ES_RIGHT
  - ES_SAVESEL
  - ES_SELFIME
  - ES_SUNKEN
  - ES_VERTICAL
  - ES_WANTRETURN (don't know how to do WM_GETDLGCODE part)
  - WS_SETFONT
  - WS_HSCROLL
  - WS_VSCROLL
*/

/*
 * RICHED20 TODO (incomplete):
 *
 * - font caching
 * - add remaining CHARFORMAT/PARAFORMAT fields
 * - right/center align should strip spaces from the beginning
 * - more advanced navigation (Ctrl-arrows, PageUp/PageDn)
 * - tabs
 * - pictures (not just smiling faces that lack API support ;-) )
 * - OLE objects
 * - calculate heights of pictures (half-done)
 * - EM_STREAMIN/EM_STREAMOUT
 * - horizontal scrolling (not even started)
 * - fix scrollbars and refresh (it sucks bigtime)
 * - hysteresis during wrapping (related to scrollbars appearing/disappearing)
 * - should remember maximum row width for wrap hysteresis
 * - find/replace
 * - how to implement EM_FORMATRANGE and EM_DISPLAYBAND ? (Mission Impossible)
 * - italic cursor with italic fonts
 * - IME
 * - most notifications aren't sent at all (the most important ones are)
 * - when should EN_SELCHANGE be sent after text change ? (before/after EN_UPDATE?)
 * - WM_SETTEXT may use wrong style (but I'm 80% sure it's OK)
 * - EM_GETCHARFORMAT with SCF_SELECTION may not behave 100% like in original (but very close)
 * - bugs in end-of-text handling (the gray bar) could get me in jail ;-)
 * - determination of row size
 * - end-of-paragraph marks should be of reasonable size
 *
 * Bugs that are probably fixed, but not so easy to verify:
 * - EN_UPDATE/EN_CHANGE are handled very incorrectly (should be OK now)
 * - undo for ME_JoinParagraphs doesn't store paragraph format ? (it does)
 * - check/fix artificial EOL logic (bCursorAtEnd, hardly logical)
 * - caret shouldn't be displayed when selection isn't empty
 * - check refcounting in style management functions (looks perfect now, but no bugs is suspicious)
 * - undo for setting default format (done, might be buggy)
 * - styles might be not released properly (looks like they work like charm, but who knows?
 *
 */

#include "editor.h"
#include <ole2.h>
#include <richole.h>
#include <winreg.h>
#define NO_SHLWAPI_STREAM 
#include <shlwapi.h>
 
WINE_DEFAULT_DEBUG_CHANNEL(richedit);

int me_debug = 0;
HANDLE me_heap = NULL;

void DoWrap(ME_TextEditor *editor) {
  HDC hDC = GetDC(editor->hWnd);
  ME_DisplayItem *item;
  ME_Context c;
  HWND hWnd = editor->hWnd;
  int yLength = editor->nTotalLength;
  int nSelFrom, nSelTo;
  int nMinSel, nMaxSel;
  
  ME_GetSelection(editor, &nSelFrom, &nSelTo);
  
  nMinSel = nSelFrom < editor->nOldSelFrom ? nSelFrom : editor->nOldSelFrom;
  nMaxSel = nSelTo > editor->nOldSelTo ? nSelTo : editor->nOldSelTo;
  
  ME_InitContext(&c, editor, hDC);
  c.pt.x = 0;
  c.pt.y = 0;
  item = editor->pBuffer->pFirst->next;
  while(item != editor->pBuffer->pLast) {
    int para_from, para_to;
    BOOL bRedraw = FALSE;
    
    para_from = item->member.para.nCharOfs;
    para_to = item->member.para.next_para->member.para.nCharOfs;
    
    if (para_from <= nMaxSel && para_to >= nMinSel && nMinSel != nMaxSel)
      bRedraw = TRUE;
    
    assert(item->type == diParagraph);
    if (!(item->member.para.nFlags & MEPF_WRAPPED)
     || (item->member.para.nYPos != c.pt.y))
      bRedraw = TRUE;
    item->member.para.nYPos = c.pt.y;
    
    ME_WrapTextParagraph(&c, item);

    if (bRedraw) {
      item->member.para.nFlags |= MEPF_REDRAW;
    }
    c.pt.y = item->member.para.nYPos + item->member.para.nHeight;
    item = item->member.para.next_para;
  }
  editor->sizeWindow.cx = c.rcView.right-c.rcView.left;
  editor->sizeWindow.cy = c.rcView.bottom-c.rcView.top;
  editor->nTotalLength = c.pt.y-c.rcView.top;
  
  ME_UpdateScrollBar(editor, -1);
  ME_EnsureVisible(editor, editor->pCursors[0].pRun);
  
  /* FIXME this should be marked for update too somehow, so that painting happens in ME_PaintContent */
  if (yLength != c.pt.y-c.rcView.top) {
    RECT rc;
    rc.left = c.rcView.left;
    rc.right = c.rcView.right;
    rc.top = c.pt.y;
    rc.bottom = c.rcView.bottom;
    InvalidateRect(editor->hWnd, &rc, FALSE);
    UpdateWindow(editor->hWnd);
  }
  
  editor->nOldSelFrom = nSelFrom;
  editor->nOldSelTo = nSelTo;
  /* PatBlt(hDC, 0, c.pt.y, c.rcView.right, c.rcView.bottom, BLACKNESS);*/

  ME_DestroyContext(&c);
  ReleaseDC(hWnd, hDC);
}

ME_TextBuffer *ME_MakeText() {
  
  ME_TextBuffer *buf = ALLOC_OBJ(ME_TextBuffer);

  ME_DisplayItem *p1 = ME_MakeDI(diTextStart);
  ME_DisplayItem *p2 = ME_MakeDI(diTextEnd);
  
  p1->prev = NULL;
  p1->next = p2;
  p2->prev = p1;
  p2->next = NULL;
  p1->member.para.next_para = p2;
  p2->member.para.prev_para = p1;
  p2->member.para.nCharOfs = 0;  
  
  buf->pFirst = p1;
  buf->pLast = p2;
  buf->pCharStyle = NULL;
  
  return buf;
}

ME_TextEditor *ME_MakeEditor(HWND hWnd) {
  ME_TextEditor *ed = ALLOC_OBJ(ME_TextEditor);
  HDC hDC;
  ed->hWnd = hWnd;
  ed->pBuffer = ME_MakeText();
  hDC = GetDC(hWnd);
  ME_MakeFirstParagraph(hDC, ed->pBuffer);
  ReleaseDC(hWnd, hDC);
  ed->bCaretShown = FALSE;
  ed->nCursors = 3;
  ed->pCursors = ALLOC_N_OBJ(ME_Cursor, ed->nCursors);
  ed->pCursors[0].pRun = ME_FindItemFwd(ed->pBuffer->pFirst, diRun);
  ed->pCursors[0].nOffset = 0;
  ed->pCursors[1].pRun = ME_FindItemFwd(ed->pBuffer->pFirst, diRun);
  ed->pCursors[1].nOffset = 0;
  ed->nTotalLength = 0;
  ed->nScrollPos = 0;
  ed->nUDArrowX = -1;
  ed->nSequence = 0;
  ed->rgbBackColor = -1;
  ed->bCaretAtEnd = FALSE;
  ed->nEventMask = 0;
  ed->nModifyStep = 0;
  ed->pUndoStack = ed->pRedoStack = NULL;
  ed->nUndoMode = umAddToUndo;
  ed->nParagraphs = 1;
  ME_CheckCharOffsets(ed);
  return ed;
}

void ME_DestroyEditor(ME_TextEditor *editor)
{
  ME_DisplayItem *pFirst = editor->pBuffer->pFirst;
  ME_DisplayItem *p = pFirst, *pNext = NULL;
  
  ME_ClearTempStyle(editor);
  ME_EmptyUndoStack(editor);
  while(p) {
    pNext = p->next;
    ME_DestroyDisplayItem(p);    
    p = pNext;
  }
  ME_ReleaseStyle(editor->pBuffer->pDefaultStyle);
    
  FREE_OBJ(editor);
}

#define UNSUPPORTED_MSG(e) \
  case e: \
    FIXME(#e ": stub\n"); \
    return DefWindowProcW(hWnd, msg, wParam, lParam);

/******************************************************************
 *        RichEditANSIWndProc (RICHED20.10)
 */
LRESULT WINAPI RichEditANSIWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  HDC hDC;
  PAINTSTRUCT ps;
  SCROLLINFO si;
  ME_TextEditor *editor = (ME_TextEditor *)GetWindowLongW(hWnd, 0);
  switch(msg) {
  
  UNSUPPORTED_MSG(EM_AUTOURLDETECT)
  UNSUPPORTED_MSG(EM_CANPASTE)
  UNSUPPORTED_MSG(EM_CHARFROMPOS)
  UNSUPPORTED_MSG(EM_DISPLAYBAND)
  UNSUPPORTED_MSG(EM_EXLIMITTEXT)
  UNSUPPORTED_MSG(EM_EXLINEFROMCHAR)
  UNSUPPORTED_MSG(EM_FINDTEXT)
  UNSUPPORTED_MSG(EM_FINDTEXTEX)
  UNSUPPORTED_MSG(EM_FINDWORDBREAK)
  UNSUPPORTED_MSG(EM_FMTLINES)
  UNSUPPORTED_MSG(EM_FORMATRANGE)
  UNSUPPORTED_MSG(EM_GETFIRSTVISIBLELINE)
  UNSUPPORTED_MSG(EM_GETIMECOMPMODE)
  /* UNSUPPORTED_MSG(EM_GETIMESTATUS) missing in Wine headers */
  UNSUPPORTED_MSG(EM_GETLANGOPTIONS)
  UNSUPPORTED_MSG(EM_GETLIMITTEXT)
  UNSUPPORTED_MSG(EM_GETLINE)
  UNSUPPORTED_MSG(EM_GETLINECOUNT)
  /* UNSUPPORTED_MSG(EM_GETOLEINTERFACE) separate stub */
  UNSUPPORTED_MSG(EM_GETOPTIONS)
  UNSUPPORTED_MSG(EM_GETRECT)
  UNSUPPORTED_MSG(EM_GETREDONAME)
  UNSUPPORTED_MSG(EM_GETTEXTMODE)
  UNSUPPORTED_MSG(EM_GETUNDONAME)
  UNSUPPORTED_MSG(EM_GETWORDBREAKPROC)
  UNSUPPORTED_MSG(EM_GETWORDBREAKPROCEX)
  UNSUPPORTED_MSG(EM_HIDESELECTION)
  UNSUPPORTED_MSG(EM_LIMITTEXT) /* also known as EM_SETLIMITTEXT */
  UNSUPPORTED_MSG(EM_LINEFROMCHAR)
  UNSUPPORTED_MSG(EM_LINEINDEX)
  UNSUPPORTED_MSG(EM_LINELENGTH)
  UNSUPPORTED_MSG(EM_LINESCROLL)
  UNSUPPORTED_MSG(EM_PASTESPECIAL)
/*  UNSUPPORTED_MSG(EM_POSFROMCHARS) missing in Wine headers */
  UNSUPPORTED_MSG(EM_REQUESTRESIZE)
  UNSUPPORTED_MSG(EM_SCROLL)
  UNSUPPORTED_MSG(EM_SCROLLCARET)
  UNSUPPORTED_MSG(EM_SELECTIONTYPE)
  UNSUPPORTED_MSG(EM_SETLANGOPTIONS)
  UNSUPPORTED_MSG(EM_SETOLECALLBACK)
  UNSUPPORTED_MSG(EM_SETOPTIONS)
  UNSUPPORTED_MSG(EM_SETRECT)
  UNSUPPORTED_MSG(EM_SETRECTNP)
  UNSUPPORTED_MSG(EM_SETTARGETDEVICE)
  UNSUPPORTED_MSG(EM_SETTEXTMODE)
  UNSUPPORTED_MSG(EM_SETUNDOLIMIT)
  UNSUPPORTED_MSG(EM_SETWORDBREAKPROC)
  UNSUPPORTED_MSG(EM_SETWORDBREAKPROCEX)
  UNSUPPORTED_MSG(EM_STREAMIN)
  UNSUPPORTED_MSG(EM_STREAMOUT)
  UNSUPPORTED_MSG(WM_SETFONT)
  UNSUPPORTED_MSG(WM_PASTE)
  UNSUPPORTED_MSG(WM_STYLECHANGING)
  UNSUPPORTED_MSG(WM_STYLECHANGED)
/*  UNSUPPORTED_MSG(WM_UNICHAR) FIXME missing in Wine headers */
    
  
  case WM_GETDLGCODE:
  {
    UINT code = DLGC_WANTCHARS|DLGC_WANTARROWS;
    if (GetWindowLongW(hWnd, GWL_STYLE)&ES_WANTRETURN)
      code |= 0; /* FIXME what can we do here ? ask for messages and censor them ? */
    return code;
  }
  case WM_NCCREATE:
  {
    CREATESTRUCTW *pcs = (CREATESTRUCTW *)lParam;
    editor = ME_MakeEditor(hWnd);
    SetWindowLongW(hWnd, 0, (long)editor);
    pcs = 0; /* ignore */
    return TRUE;
  }
  case EM_EMPTYUNDOBUFFER:
    ME_EmptyUndoStack(editor);
    return 0;
  case EM_GETSEL:
  {
    ME_GetSelection(editor, (int *)wParam, (int *)lParam);
    if (!((wParam|lParam) & 0xFFFF0000))
      return (lParam<<16)|wParam;
    return -1;
  }
  case EM_EXGETSEL:
  {
    CHARRANGE *pRange = (CHARRANGE *)lParam;
    ME_GetSelection(editor, (int *)&pRange->cpMin, (int *)&pRange->cpMax);
    return 0;
  }
  case EM_CANUNDO:
    return editor->pUndoStack != NULL;
  case EM_CANREDO:
    return editor->pRedoStack != NULL;
  case EM_UNDO:
    ME_Undo(editor);
    return 0;
  case EM_REDO:
    ME_Redo(editor);
    return 0;
  case EM_SETSEL:
  {
    ME_SetSelection(editor, wParam, lParam);
    ME_Repaint(editor);
    ME_SendSelChange(editor);
    return 0;
  }
  case EM_EXSETSEL:
  {
    CHARRANGE *pRange = (CHARRANGE *)lParam;
    ME_SetSelection(editor, pRange->cpMin, pRange->cpMax);
    /* FIXME optimize */
    ME_Repaint(editor);
    ME_SendSelChange(editor);
    return 0;
  }
  case EM_SETBKGNDCOLOR:
  {
    LRESULT lColor = ME_GetBackColor(editor);
    if (wParam)
      editor->rgbBackColor = -1;
    else
      editor->rgbBackColor = lParam; 
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return lColor;
  }
  case EM_GETMODIFY:
    return editor->nModifyStep == 0 ? 0 : 1;
  case EM_SETMODIFY:
  {
    if (wParam)
      editor->nModifyStep = 0x80000000;
    else
      editor->nModifyStep = 0;
    
    return 0;
  }
  case EM_SETREADONLY:
  {
    long nStyle = GetWindowLongW(hWnd, GWL_STYLE);
    if (wParam)
      nStyle |= ES_READONLY;
    else
      nStyle &= ~ES_READONLY;
    SetWindowLongW(hWnd, GWL_STYLE, nStyle);
    ME_Repaint(editor);
    return 0;
  }
  case EM_SETEVENTMASK:
    editor->nEventMask = lParam;
    return 0;
  case EM_GETEVENTMASK:
    return editor->nEventMask;
  case EM_SETCHARFORMAT:
  {
    CHARFORMAT2W buf, *p;
    p = ME_ToCF2W(&buf, (CHARFORMAT2W *)lParam);
    if (!wParam)
      ME_SetDefaultCharFormat(editor, p);
    else if (wParam == (SCF_WORD | SCF_SELECTION))
      FIXME("word selection not supported\n");
    else if (wParam == SCF_ALL)
      ME_SetCharFormat(editor, 0, ME_GetTextLength(editor), p);
    else
      ME_SetSelectionCharFormat(editor, p);
    ME_CommitUndo(editor);
    ME_UpdateRepaint(editor);
    return 0;
  }
  case EM_GETCHARFORMAT:
  {
    CHARFORMAT2W tmp;
    tmp.cbSize = sizeof(tmp);
    if (!wParam)
      ME_GetDefaultCharFormat(editor, &tmp);
    else
      ME_GetSelectionCharFormat(editor, &tmp);
    ME_CopyToCFAny((CHARFORMAT2W *)lParam, &tmp);
    return 0;
  }
  case EM_SETPARAFORMAT:
    ME_SetSelectionParaFormat(editor, (PARAFORMAT2 *)lParam);
    ME_CommitUndo(editor);
    return 0;
  case EM_GETPARAFORMAT:
    ME_GetSelectionParaFormat(editor, (PARAFORMAT2 *)lParam);
    return 0;
  case WM_CLEAR:
  {
    int from, to;
    ME_GetSelection(editor, &from, &to);
    ME_InternalDeleteText(editor, from, to-from);
    ME_CommitUndo(editor);
    ME_UpdateRepaint(editor);
    return 0;
  }
  case EM_REPLACESEL:
  {
    int from, to;
    ME_Cursor c;
    ME_Style *style;
    LPWSTR wszText = ME_ToUnicode(hWnd, (void *)lParam);
    size_t len = lstrlenW(wszText);
    
    ME_GetSelection(editor, &from, &to);
    ME_CursorFromCharOfs(editor, from, &c);
    if (from != to) {
      style = c.pRun->member.run.style;
      ME_AddRefStyle(style); /* ME_GetInsertStyle has already done that */
    }
    else
      style = ME_GetInsertStyle(editor, 0);
    ME_InternalDeleteText(editor, from, to-from);
    ME_InsertTextFromCursor(editor, 0, wszText, len, style);
    ME_ReleaseStyle(style);
    ME_EndToUnicode(hWnd, wszText);
    /* drop temporary style if line end */
    /* FIXME question: does abc\n mean: put abc, clear temp style, put \n? (would require a change) */  
    if (len>0 && wszText[len-1] == '\n')
      ME_ClearTempStyle(editor);
      
    ME_CommitUndo(editor);
    if (!wParam)
      ME_EmptyUndoStack(editor);
    ME_UpdateRepaint(editor);
    return 0;
  }
  case WM_SETTEXT:
  {
    LPWSTR wszText = ME_ToUnicode(hWnd, (void *)lParam);
    ME_InternalDeleteText(editor, 0, ME_GetTextLength(editor));
    /* uses default style! */
    ME_InsertTextFromCursor(editor, 0, wszText, -1, editor->pBuffer->pDefaultStyle);
    ME_EndToUnicode(hWnd, wszText);
    ME_CommitUndo(editor);
    ME_EmptyUndoStack(editor);
    ME_UpdateRepaint(editor);
    return 0;
  }
  case WM_CUT:
  case WM_COPY:
  {
    int from, to, pars;
    WCHAR *data;
    HANDLE hData;
    
    if (!OpenClipboard(hWnd))
      return 0;
      
    EmptyClipboard();
    ME_GetSelection(editor, &from, &to);
    pars = ME_CountParagraphsBetween(editor, from, to);
    hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR)*(to-from+pars+1));
    data = (WCHAR *)GlobalLock(hData);
    ME_GetTextW(editor, data, from, to-from, TRUE);
    GlobalUnlock(hData);
    SetClipboardData(CF_UNICODETEXT, hData);
    CloseClipboard();
    if (msg == WM_CUT)
    {
      ME_InternalDeleteText(editor, from, to-from);
      ME_CommitUndo(editor);
      ME_UpdateRepaint(editor);
    }
    return 0;
  }
  case WM_GETTEXTLENGTH:
    return ME_GetTextLength(editor);
  case WM_GETTEXT:
  {
    TEXTRANGEW tr; /* W and A differ only by rng->lpstrText */
    tr.chrg.cpMin = 0;
    tr.chrg.cpMax = wParam-1;
    tr.lpstrText = (WCHAR *)lParam;
    return RichEditANSIWndProc(hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
  }
  case EM_GETSELTEXT:
  {
    int from, to;
    TEXTRANGEW tr; /* W and A differ only by rng->lpstrText */
    ME_GetSelection(editor, &from, &to);
    tr.chrg.cpMin = from;
    tr.chrg.cpMax = to;
    tr.lpstrText = (WCHAR *)lParam;
    return RichEditANSIWndProc(hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
  }
  case EM_GETTEXTRANGE:
  {
    TEXTRANGEW *rng = (TEXTRANGEW *)lParam;
    if (IsWindowUnicode(hWnd))
      return ME_GetTextW(editor, rng->lpstrText, rng->chrg.cpMin, rng->chrg.cpMax-rng->chrg.cpMin, FALSE);
    else
    {
      int nLen = rng->chrg.cpMax-rng->chrg.cpMin;
      WCHAR *p = ALLOC_N_OBJ(WCHAR, nLen+1);
      int nChars = ME_GetTextW(editor, p, rng->chrg.cpMin, nLen, FALSE);
      /* FIXME this is a potential security hole (buffer overrun) 
         if you know more about wchar->mbyte conversion please explain
      */
      WideCharToMultiByte(CP_ACP, 0, p, nChars+1, (char *)rng->lpstrText, nLen+1, NULL, NULL);
      FREE_OBJ(p);
      return nChars;
    }
    return ME_GetTextW(editor, rng->lpstrText, rng->chrg.cpMin, rng->chrg.cpMax-rng->chrg.cpMin, FALSE);
  }
  case WM_CREATE:
    ME_CommitUndo(editor);
/*    ME_InsertTextFromCursor(editor, 0, (WCHAR *)L"x", 1, editor->pBuffer->pDefaultStyle); */
    DoWrap(editor);
    ME_MoveCaret(editor);
    return 0;
  case WM_DESTROY:
    ME_DestroyEditor(editor);
    SetWindowLongW(hWnd, 0, 0);
    PostQuitMessage(0);
    break;
  case WM_LBUTTONDOWN:
    SetFocus(hWnd);
    ME_LButtonDown(editor, (short)LOWORD(lParam), (short)HIWORD(lParam));
    SetCapture(hWnd);
    break;
  case WM_MOUSEMOVE:
    if (GetCapture() == hWnd)
      ME_MouseMove(editor, (short)LOWORD(lParam), (short)HIWORD(lParam));
    break;
  case WM_LBUTTONUP:
    if (GetCapture() == hWnd)
      ReleaseCapture();
    break;
  case WM_PAINT:
    hDC = BeginPaint(hWnd, &ps);
    ME_PaintContent(editor, hDC, FALSE);
    EndPaint(hWnd, &ps);
    break;
  case WM_SETFOCUS:
    ME_ShowCaret(editor);
    ME_SendOldNotify(editor, EN_SETFOCUS);
    return 0;
  case WM_KILLFOCUS:
    ME_HideCaret(editor);
    ME_SendOldNotify(editor, EN_KILLFOCUS);
    return 0;
  case WM_ERASEBKGND:
  {
    HDC hDC = (HDC)wParam;
    RECT rc;
    COLORREF rgbBG = ME_GetBackColor(editor);
    if (GetUpdateRect(hWnd,&rc,TRUE))
    {
      HBRUSH hbr = CreateSolidBrush(rgbBG);
      FillRect(hDC, &rc, hbr);
      DeleteObject(hbr);
    }
    return 1;
  }
  case WM_COMMAND:
    TRACE("editor wnd command = %d\n", LOWORD(wParam));
    return 0;
  case WM_KEYDOWN:
    if (ME_ArrowKey(editor, LOWORD(wParam), GetKeyState(VK_CONTROL)<0)) {
      ME_CommitUndo(editor);
      ME_EnsureVisible(editor, editor->pCursors[0].pRun);
      HideCaret(hWnd);
      ME_MoveCaret(editor);
      ShowCaret(hWnd);
      return 0;
    }
    if (GetKeyState(VK_CONTROL)<0)
    {
      if (LOWORD(wParam)=='W')
      {
        CHARFORMAT2W chf;
        char buf[2048];
        ME_GetSelectionCharFormat(editor, &chf);
        ME_DumpStyleToBuf(&chf, buf);
        MessageBoxA(NULL, buf, "Style dump", MB_OK);
      }
      if (LOWORD(wParam)=='Q')
      {
        ME_CheckCharOffsets(editor);
      }
    }
    goto do_default;
  case WM_CHAR: 
  {
    WCHAR wstr;
    if (GetWindowLongW(editor->hWnd, GWL_STYLE) & ES_READONLY) {
      MessageBeep(MB_ICONERROR);
      return 0; /* FIXME really 0 ? */
    }
    wstr = LOWORD(wParam);
    if (((unsigned)wstr)>=' ' || wstr=='\r') {
      /* FIXME maybe it would make sense to call EM_REPLACESEL instead ? */
      ME_Style *style = ME_GetInsertStyle(editor, 0);
      ME_SaveTempStyle(editor);
      ME_InsertTextFromCursor(editor, 0, &wstr, 1, style);
      ME_ReleaseStyle(style);
      ME_CommitUndo(editor);
      ME_UpdateRepaint(editor);
    }
    return 0;
  }
  case WM_VSCROLL: 
  {
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS;
    GetScrollInfo(hWnd, SB_VERT, &si);
    switch(LOWORD(wParam)) {
    case SB_THUMBTRACK:
      SetScrollPos(hWnd, SB_VERT, si.nTrackPos, FALSE);
      ScrollWindow(hWnd, 0, si.nPos-si.nTrackPos, NULL, NULL);
      /* InvalidateRect(hWnd, NULL, TRUE); */
      UpdateWindow(hWnd);
      break;
    }
    break;
  }
  case WM_SIZE:
  {
    ME_DisplayItem *tp = editor->pBuffer->pFirst;
    while(tp)
    {
      if (tp->type == diParagraph)
      {
        tp->member.para.nFlags &= ~MEPF_WRAPPED;
        tp = tp->member.para.next_para;
      }
      else
        tp = tp->next;
    }
    ME_Repaint(editor);
    return DefWindowProcW(hWnd, msg, wParam, lParam);
  }
  case EM_GETOLEINTERFACE:
    FIXME("EM_GETOLEINTERFACE: stub\n");
    /* return 0x12345678; to find out if the missing interface is
       related to app crash */ 
    return 0;
  default:
  do_default:
    return DefWindowProcW(hWnd, msg, wParam, lParam);
  }
  return 0L;
}

/******************************************************************
 *        RichEdit10ANSIWndProc (RICHED20.9)
 */
LRESULT WINAPI RichEdit10ANSIWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  /* FIXME: this is NOT the same as 2.0 version */
  return RichEditANSIWndProc(hWnd, msg, wParam, lParam);
}

void ME_SendOldNotify(ME_TextEditor *editor, int nCode)
{
  HWND hWnd = editor->hWnd;
  SendMessageA(GetParent(hWnd), WM_COMMAND, (nCode<<16)|GetWindowLongW(hWnd, GWLP_ID), (LPARAM)hWnd);
}

int ME_CountParagraphsBetween(ME_TextEditor *editor, int from, int to)
{
  ME_DisplayItem *item = ME_FindItemFwd(editor->pBuffer->pFirst, diParagraph);
  int i = 0;
  
  while(item && item->member.para.next_para->member.para.nCharOfs <= from)
    item = item->member.para.next_para;
  if (!item)
    return 0;
  while(item && item->member.para.next_para->member.para.nCharOfs <= to) {
    item = item->member.para.next_para;
    i++;
  }
  return i;
}

int ME_GetTextW(ME_TextEditor *editor, WCHAR *buffer, int nStart, int nChars, int bCRLF)
{
  ME_DisplayItem *item = ME_FindItemFwd(editor->pBuffer->pFirst, diParagraph);
  int nWritten = 0;
  
  while(item && item->member.para.next_para->member.para.nCharOfs <= nStart)
    item = ME_FindItemFwd(item, diParagraph);
  if (!item) {
    *buffer = L'\0';
    return 0;
  }
  nStart -= item->member.para.nCharOfs;

  do {  
    item = ME_FindItemFwd(item, diRun);
  } while(item && (item->member.run.nCharOfs + ME_StrLen(item->member.run.strText) <= nStart));
  assert(item);    
  
  nStart -= item->member.run.nCharOfs;
  
  if (nStart)
  {
    int nLen = ME_StrLen(item->member.run.strText) - nStart;
    if (nLen > nChars)
      nLen = nChars;
    CopyMemory(buffer, item->member.run.strText->szData + nStart, sizeof(WCHAR)*nLen);
    nChars -= nLen;
    nWritten += nLen;
    if (!nChars)
      return nWritten;
    buffer += nLen;
    nStart = 0;
    item = ME_FindItemFwd(item, diRun);
  }
  
  while(nChars && item)
  {
    int nLen = ME_StrLen(item->member.run.strText);
    if (nLen > nChars)
      nLen = nChars;
      
    if (item->member.run.nFlags & MERF_ENDPARA)
    {
      if (bCRLF) {
        *buffer++ = '\r';
        nWritten++;
      }        
      *buffer = '\n';
      assert(nLen == 1);
    }
    else      
      CopyMemory(buffer, item->member.run.strText->szData, sizeof(WCHAR)*nLen);
    nChars -= nLen;
    nWritten += nLen;
    buffer += nLen;    
      
    if (!nChars)
    {
      *buffer = L'\0';
      return nWritten;
    }
    item = ME_FindItemFwd(item, diRun);
  }
  *buffer = L'\0';
  return nWritten;  
}

static WCHAR wszClassName[] = {'R', 'i', 'c', 'h', 'E', 'd', 'i', 't', '2', '0', 'W', 0};
static WCHAR wszClassName50[] = {'R', 'i', 'c', 'h', 'E', 'd', 'i', 't', '5', '0', 'W', 0};

void ME_RegisterEditorClass(HINSTANCE hInstance)
{
  BOOL bResult;
  WNDCLASSW wcW;
  WNDCLASSA wcA;
  
  wcW.style = CS_HREDRAW | CS_VREDRAW;
  wcW.lpfnWndProc = RichEditANSIWndProc;
  wcW.cbClsExtra = 0;
  wcW.cbWndExtra = 4;
  wcW.hInstance = NULL; /* hInstance would register DLL-local class */
  wcW.hIcon = NULL;
  wcW.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_IBEAM));
  wcW.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
  wcW.lpszMenuName = NULL;
  wcW.lpszClassName = wszClassName;
  bResult = RegisterClassW(&wcW);  
  assert(bResult);
  wcW.lpszClassName = wszClassName50;
  bResult = RegisterClassW(&wcW);  
  assert(bResult);

  wcA.style = CS_HREDRAW | CS_VREDRAW;
  wcA.lpfnWndProc = RichEditANSIWndProc;
  wcA.cbClsExtra = 0;
  wcA.cbWndExtra = 4;
  wcA.hInstance = NULL; /* hInstance would register DLL-local class */
  wcA.hIcon = NULL;
  wcA.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_IBEAM));
  wcA.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
  wcA.lpszMenuName = NULL;
  wcA.lpszClassName = "RichEdit20A";
  bResult = RegisterClassA(&wcA);  
  assert(bResult);
  wcA.lpszClassName = "RichEdit50A";
  bResult = RegisterClassA(&wcA);  
  assert(bResult);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    TRACE("\n");
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hinstDLL);
      me_heap = HeapCreate (0, 0x10000, 0);
      ME_RegisterEditorClass(hinstDLL);
      break;

    case DLL_PROCESS_DETACH:
      UnregisterClassW(wszClassName, hinstDLL);
      UnregisterClassA("RichEdit20A", hinstDLL);
      HeapDestroy (me_heap);
      me_heap = NULL;
      break;
    }
    return TRUE;
}

/******************************************************************
 *        CreateTextServices (RICHED20.4)
 *
 * FIXME should be ITextHost instead of void*
 */
HRESULT WINAPI CreateTextServices(IUnknown *punkOuter, void *pITextHost,
    IUnknown **ppUnk)
{
  FIXME("stub\n");
  /* FIXME should support aggregation */
  if (punkOuter)
    return CLASS_E_NOAGGREGATION;
    
  return E_FAIL; /* E_NOTIMPL isn't allowed by MSDN */
}

/******************************************************************
 *        REExtendedRegisterClass (RICHED20.8)
 *
 * FIXME undocumented
 */
void WINAPI REExtendedRegisterClass(void)
{
  FIXME("stub\n");
}
