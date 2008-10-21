/* Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
 * OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder
 * shall not be used in advertising or otherwise to promote the sale, use
 * or other dealings in this Software without prior written authorization
 * of the copyright holder.
 */ 

#pragma ident   "@(#)Xtsol.c 1.7     08/08/12 SMI"

#define NEED_REPLIES
#define NEED_EVENTS

#include <stdio.h>
#include <sys/types.h>
#include <tsol/label.h>
#include <sys/tsol/label_macro.h>

#include <X11/Xlibint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/extutil.h>

#define _XTSOL_SERVER

#include "Xtsolproto.h"
#include "Xtsol.h"

#define SL_SIZE (sizeof (blevel_t))


/*
 * Initialiazation routine
 */
static int TsolInitialized = False;
static int X_TsolExtensionCode;

#define TsolCheckExtension(dpy)\
    {\
        if (!TsolInitialized)\
            if (InitializeTsol(dpy) == 0)\
            {\
                return 0;\
            }\
    }

static int
InitializeTsol(dpy)
    register Display *dpy;
{
    int tmp;
    int event;

    if (XQueryExtension(dpy, TSOLNAME, &X_TsolExtensionCode, &event, &tmp))
    {
        TsolInitialized = True;
	return 1;
    }
    else 
    {
        TsolInitialized = False;
        return 0; /* Failure */
    }
}

/*
 * returns true if win is trusted path
 */
Bool
XTSOLIsWindowTrusted(dpy, win)
    Display *dpy;
    Window win;
{
    XTsolClientAttributes    clientattr;
    Status result;
    extern Status XTSOLgetClientAttributes(Display *, XID, XTsolClientAttributes *);

    TsolCheckExtension(dpy);

    result = XTSOLgetClientAttributes(dpy, win, &clientattr);

    return (result ? clientattr.trustflag : False);
}

Status
XTSOLsetPolyInstInfo(dpy, sl, uidp, enabled)
    register Display *dpy;
    m_label_t *sl;
    uid_t *uidp;
    int enabled;
{
    register xSetPolyInstInfoReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetPolyInstInfo, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetPolyInstInfo;
    req->uid = (CARD32 )*uidp;
    req->enabled = (CARD32 )enabled;
    req->sllength = (CARD16 )SL_SIZE;
    req->length = (CARD16 ) (4 + ((int)(req->sllength) / 4));
    Data(dpy, (char *)sl, SL_SIZE);

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

Status
XTSOLsetPropLabel(dpy, win, property, sl)
    register Display *dpy;
    register Window win;
    Atom property;
    m_label_t *sl;
{
    register xSetPropLabelReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetPropLabel, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetPropLabel;
    req->id = win;
    req->atom = property;
    req->labelType = (CARD16) RES_SL;
    req->sllength = SL_SIZE;
    req->illength = 0;
    req->length = 5 + (int)(req->sllength / 4);
    Data(dpy, (char *)sl, SL_SIZE);

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

Status
XTSOLsetPropUID(dpy, win, property, uidp)
    register Display *dpy;
    register Window win;
    Atom property;
    uid_t *uidp;
{
    register xSetPropUIDReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetPropUID, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetPropUID;
    req->length = 4;
    req->id = win;
    req->atom = property;
    req->uid = *uidp;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

Status
XTSOLsetResLabel(dpy, object, resourceFlag, sl)
    register Display *dpy;
    register XID object;
    ResourceType resourceFlag;
    m_label_t *sl;
{
    register xSetResLabelReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetResLabel, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetResLabel;
    req->id = object;
    req->resourceType = (CARD16) resourceFlag;
    req->labelType = (CARD16) RES_SL;
    req->sllength = SL_SIZE;
    req->illength = 0;
    req->length = 4 + (int)(req->sllength / 4);
    Data(dpy, (char *)sl, SL_SIZE);
    
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

Status
XTSOLsetResUID(dpy, object, resourceFlag, uidp)
    register Display *dpy;
    register XID object;
    ResourceType resourceFlag;
    uid_t *uidp;
{
    register xSetResUIDReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetResUID, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetResUID;
    req->length = 4;
    req->id = object;
    req->resourceType = (CARD16) resourceFlag;
    req->uid = *uidp;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

Status
XTSOLsetSSHeight(dpy, screen_num, newHeight)
    Display *dpy;
    int screen_num;
    int newHeight;
{
    register xSetResUIDReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetResUID, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetResUID;
    req->length = 4;
    req->id = screen_num;
    req->resourceType = STRIPEHEIGHT;
    req->uid = newHeight;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

Status
XTSOLsetSessionHI(dpy, sl)
    Display *dpy;
    bclear_t *sl;
{
    register xSetResLabelReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetResLabel, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetResLabel;
    req->id = 0;
    req->resourceType = SESSIONHI;
    req->labelType = RES_SL;
    req->sllength = SL_SIZE;
    req->illength = 0;
    req->length = 4 + (int)(req->sllength / 4);
    Data(dpy, (char *)sl, SL_SIZE);

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;

}

Status
XTSOLsetSessionLO(dpy, sl)
    Display *dpy;
    m_label_t *sl;
{
    register xSetResLabelReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetResLabel, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetResLabel;
    req->id = 0;
    req->resourceType = SESSIONLO;
    req->labelType = RES_SL;
    req->sllength = SL_SIZE;
    req->illength = 0;
    req->length = 4 + (int)(req->sllength / 4);
    Data(dpy, (char *)sl, SL_SIZE);

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;

}

Status
XTSOLsetWorkstationOwner(dpy, uidp)
    Display *dpy;
    uid_t *uidp;
{    
    register xSetResUIDReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(SetResUID, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_SetResUID;
    req->length = 4;
    req->id = 0;
    req->resourceType = RES_OUID;
    req->uid = *uidp;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

/*
 * make the window a trusted path window
 */
Status
XTSOLMakeTPWindow(dpy, win) 
    Display *dpy;
    Window win;
{
    register xMakeTPWindowReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(MakeTPWindow, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_MakeTPWindow;
    req->id = win;
    req->length = 2;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;

}

/*
 * Turn on the trusted bit of window
 */
Status
XTSOLMakeTrustedWindow(dpy, win) 
    Display *dpy;
    Window win;
{
    register xMakeTrustedWindowReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(MakeTrustedWindow, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_MakeTrustedWindow;
    req->id = win;
    req->length = 2;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;

}

/*
 * Turn off the  trusted bit of window
 */
Status
XTSOLMakeUntrustedWindow(dpy, win) 
    Display *dpy;
    Window win;
{
    register xMakeUntrustedWindowReq *req;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(MakeUntrustedWindow, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_MakeUntrustedWindow;
    req->id = win;
    req->length = 2;

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;

}

/*
 * get resource attributes. 
 */
Status
XTSOLgetClientAttributes(dpy, xid, clientattr)
    register Display *dpy;
    XID xid;    /* window id of client */
    XTsolClientAttributes    *clientattr;
{
    register xGetClientAttributesReq *req;
    xGetClientAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetClientAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetClientAttributes;
    req->length = 2;
    req->id = xid;

    status = _XReply (dpy, (xReply *)&rep, 0, xTrue); 
    if (status)
    {
        /* copy the data to user struct */
        if (rep.trustflag == (BYTE)1)
            clientattr->trustflag = 1;
        else
            clientattr->trustflag = 0;
        clientattr->uid = (uid_t) rep.uid;
        clientattr->gid = (gid_t) rep.gid;
        clientattr->pid = (pid_t) rep.pid;
        clientattr->auditid = (au_id_t) rep.auditid;
        clientattr->sessionid = (u_long) rep.sessionid;
        clientattr->iaddr = (u_long) rep.iaddr;
    }
    
    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

Status
XTSOLgetClientLabel(dpy, object, sl)
    register Display *dpy;
    register XID object;
    m_label_t *sl;
{
    register xGetClientLabelReq *req;
    xGenericReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetClientLabel, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetClientLabel;
    req->length = 3;
    req->id = object;
    req->mask = RES_LABEL;

    status = _XReply (dpy, (xReply *)&rep, 0, xFalse);
    if (status)
    {
        if ((int)(rep.data00+rep.data01) > 0)
        {
            _XRead (dpy, (char *) sl, rep.data00);
        }
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

Status
XTSOLgetPropAttributes(dpy, window, property, propattrp)
    register Display *dpy;
    Window window;
    Atom property;
    XTsolPropAttributes *propattrp;
{
    register xGetPropAttributesReq *req;
    xGetPropAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetPropAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetPropAttributes;
    req->length = 4;
    req->id = window;
    req->atom = property;
    req->mask = RES_ALL;

    status = _XReply (dpy, (xReply *)&rep, 0, xFalse); 
    if (status)
    {
        /* copy the data to user struct */
        propattrp->uid = rep.uid;
        /* read the label info */
        if (rep.sllength > 0)
            _XRead (dpy, (char *) (propattrp->sl), rep.sllength);
    }
    
    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

Status
XTSOLgetPropLabel(dpy, win, property, sl)
    register Display *dpy;
    register Window win;
    Atom property;
    m_label_t *sl;
{
    register xGetPropAttributesReq *req;
    xGetPropAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetPropAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetPropAttributes;
    req->length = 4;
    req->id = win;
    req->atom = property;
    req->mask = RES_LABEL;

    status = _XReply (dpy, (xReply *)&rep, 0, xFalse); 
    if (status)
    {
        /* copy the data to user struct */
        /* read the label info */
        if ((int)(rep.sllength) > 0)
            _XRead (dpy, (char *) sl, rep.sllength);
    }
    
    UnlockDisplay(dpy);
    SyncHandle();
    return status;
    
}

Status
XTSOLgetPropUID(dpy, win, property, uidp)
    register Display *dpy;
    register Window win;
    Atom property;
    uid_t *uidp;
{
    register xGetPropAttributesReq *req;
    xGetPropAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetPropAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetPropAttributes;
    req->length = 4;
    req->id = win;
    req->atom = property;
    req->mask = RES_UID;

    status = _XReply (dpy, (xReply *)&rep, 0, xTrue); 
    if (status)
    {
        /* copy the data to user struct */
        *uidp = rep.uid;
    }
    
    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

Status
XTSOLgetResAttributes(dpy, object, resourceFlag, resattrp)
    register Display *dpy;
    register XID object;
    ResourceType resourceFlag;
    XTsolResAttributes *resattrp;
{
    register xGetResAttributesReq *req;
    xGetResAttributesReply rep;
    Status status;
    
    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetResAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetResAttributes;
    req->length = 3;
    req->id = object;
    req->resourceType = (CARD16) resourceFlag;
    req->mask = RES_ALL;

    status = _XReply (dpy, (xReply *)&rep, 0, xFalse);
    if (status)
    {
        /* copy the data to user struct */
        resattrp->uid = rep.uid;
        resattrp->ouid = rep.owneruid;
        /* read the label info */
        if (rep.sllength > 0)
            _XRead (dpy, (char *) (resattrp->sl), rep.sllength);
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

/*
 * get resource label. 
 */
Status
XTSOLgetResLabel(dpy, object, resourceFlag, sl)
    register Display *dpy;
    register XID object;
    ResourceType resourceFlag;
    m_label_t *sl;
{
    register xGetResAttributesReq *req;
    xGetResAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetResAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetResAttributes;
    req->length = 3;
    req->id = object;
    req->resourceType = (CARD16) resourceFlag;
    req->mask = RES_LABEL;

    status = _XReply (dpy, (xReply *)&rep, 0, xFalse); 
    if (status)
    {
        if ((int)(rep.sllength) > 0) 
        {
            _XRead (dpy, (char *) sl, rep.sllength);
        }
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

Status
XTSOLgetResUID(dpy, object, resourceFlag, uidp)
    register Display *dpy;
    register XID object;
    ResourceType resourceFlag;
    uid_t *uidp;
{
    register xGetResAttributesReq *req;
    xGetResAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetResAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetResAttributes;
    req->length = 3;
    req->id = object;
    req->resourceType = (CARD16) resourceFlag;
    req->mask = RES_UID;

    status = _XReply (dpy, (xReply *)&rep, 0, xTrue); 
    if (status)
    {
        /* copy the data to user struct */
        *uidp = rep.uid;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}

Status
XTSOLgetSSHeight(dpy, screen_num, newHeight)
    Display *dpy;
    int screen_num;
    int *newHeight;
{
    register xGetResAttributesReq *req;
    xGetResAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetResAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetResAttributes;
    req->length = 3;
    req->id = screen_num;
    req->resourceType = STRIPEHEIGHT;
    req->mask = RES_STRIPE;

    status = _XReply (dpy, (xReply *)&rep, 0, xTrue);
    if (status)
    {
        /* copy the data to user struct */
        *newHeight = rep.uid;
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return status;

}

Status 
XTSOLgetWorkstationOwner(dpy, uidp)
    Display *dpy;
    uid_t *uidp;
{
    register xGetResAttributesReq *req;
    xGetResAttributesReply rep;
    Status status;

    TsolCheckExtension(dpy);
    LockDisplay(dpy);
    GetReq(GetResAttributes, req);

    req->reqType = (CARD8) X_TsolExtensionCode;
    req->minorCode = X_GetResAttributes;
    req->length = 3;
    req->mask = RES_OUID;

    status = _XReply (dpy, (xReply *)&rep, 0, xTrue); 
    if (status)
    {
        /* copy the data to user struct */
        *uidp = rep.owneruid;
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return status;
}