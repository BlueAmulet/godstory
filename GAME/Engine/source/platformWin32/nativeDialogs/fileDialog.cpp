//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#define _WIN32_WINNT 0X500
#include <windows.h>
#include <ShellAPI.h>
#include "console/simBase.h"
#include "platform/nativeDialogs/fileDialog.h"
#include "platform/threads/mutex.h"
#include "platformWin32/platformWin32.h"
#include "util/safeDelete.h"
#include "math/mMath.h"
#include "core/unicode.h"
#include "console/consoleTypes.h"
#include "platform/profiler.h"
#include "core/resManager.h"
#include <ShlObj.h>
#include <WindowsX.h>
#include <Commdlg.h>
#include <Dlgs.h>



//-----------------------------------------------------------------------------
// PlatformFileDlgData Implementation
//-----------------------------------------------------------------------------
FileDialogData::FileDialogData()
{
   // Default Path
   //
   //  Try to provide consistent experience by recalling the last file path
   // - else
   //  Default to Working Directory if last path is not set or is invalid
   mDefaultPath = StringTable->insert( Con::getVariable("Tools::FileDialogs::LastFilePath") );
   if( mDefaultPath == StringTable->lookup("") || !Platform::isDirectory( mDefaultPath ) )
      mDefaultPath = Platform::getCurrentDirectory();

   mDefaultFile = StringTable->insert("");
   mFilters = StringTable->insert("");
   mFile = StringTable->insert("");
   mTitle = StringTable->insert("");

   mStyle = 0;

}
FileDialogData::~FileDialogData()
{

}

static LRESULT PASCAL OKBtnFolderHackProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   WNDPROC oldProc = (WNDPROC)GetProp(hWnd, dT("OldWndProc"));

   switch(uMsg)
   {
      case WM_COMMAND:
         if(LOWORD(wParam) == IDOK)
         {
            LPOPENFILENAME ofn = (LPOPENFILENAME)GetProp(hWnd, dT("OFN"));
            if(ofn == NULL)
               break;

            SendMessage(hWnd, CDM_GETFILEPATH, ofn->nMaxFile, (LPARAM)ofn->lpstrFile);

            char *filePath;
#ifdef UNICODE
            char fileBuf[MAX_PATH];
            convertUTF16toUTF8(ofn->lpstrFile, fileBuf, sizeof(fileBuf));
            filePath = fileBuf;
#else
            filePath = ofn->lpstrFile;
#endif

            if(Platform::isDirectory(filePath))
            {
               // Got a directory
               EndDialog(hWnd, IDOK);
            }
         }
         break;
   }

   if(oldProc)
      return CallWindowProc(oldProc, hWnd, uMsg, wParam, lParam);
   else
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static UINT_PTR CALLBACK FolderHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
   HWND hParent = GetParent(hdlg);
   
   switch(uMsg)
   {
      case WM_INITDIALOG:
         {
            LPOPENFILENAME lpofn = (LPOPENFILENAME)lParam;


            SendMessage(hParent, CDM_SETCONTROLTEXT, stc3, (LPARAM)dT("Folder name:"));
            SendMessage(hParent, CDM_HIDECONTROL, cmb1, 0);
            SendMessage(hParent, CDM_HIDECONTROL, stc2, 0);

            LONG oldProc = SetWindowLong(hParent, GWL_WNDPROC, (LONG)OKBtnFolderHackProc);
            SetProp(hParent, dT("OldWndProc"), (HANDLE)oldProc);
            SetProp(hParent, dT("OFN"), (HANDLE)lpofn);
         }
         break;

      case WM_NOTIFY:
         {
            LPNMHDR nmhdr = (LPNMHDR)lParam;
            switch(nmhdr->code)
            {
               case CDN_FOLDERCHANGE:
                  {
                     LPOFNOTIFY lpofn = (LPOFNOTIFY)lParam;

                     OpenFolderDialog *ofd = (OpenFolderDialog *)lpofn->lpOFN->lCustData;
                     
#ifdef UNICODE
                     UTF16 buf[MAX_PATH];
#else
                     char buf[MAX_PATH];
#endif

                     SendMessage(hParent, CDM_GETFOLDERPATH, sizeof(buf), (LPARAM)buf);

                     char filePath[MAX_PATH];
#ifdef UNICODE
                     convertUTF16toUTF8(buf, filePath, sizeof(filePath));
#else
                     dStrcpy( filePath, sizeof(filePath), buf );
#endif

                     // [tom, 12/8/2006] Hack to remove files from the list because
                     // CDN_INCLUDEITEM doesn't work for regular files and folders.
                     HWND shellView = GetDlgItem(hParent, lst2);
                     HWND listView = FindWindowEx(shellView, 0, WC_LISTVIEW, NULL);
                     if(listView)
                     {
                        S32 count = ListView_GetItemCount(listView);
                        for(S32 i = count - 1;i >= 0;--i)
                        {
                           ListView_GetItemText(listView, i, 0, buf, sizeof(buf));
                           
#ifdef UNICODE
                           char buf2[MAX_PATH];
                           convertUTF16toUTF8(buf, buf2, sizeof(buf2));
#else
                           char *buf2 = buf;
#endif
                           char full[MAX_PATH];
                           dSprintf(full, sizeof(full), "%s\\%s", filePath, buf2);

                           if(!Platform::isDirectory(full))
                           {
                              ListView_DeleteItem(listView, i);
                           }
                        }
                     }

                     if(ofd->mMustExistInDir == NULL || *ofd->mMustExistInDir == 0)
                        break;

                     HWND hOK = GetDlgItem(hParent, IDOK);
                     if(hOK == NULL)
                        break;

                     char checkPath[MAX_PATH];
                     dSprintf(checkPath, sizeof(checkPath), "%s\\%s", filePath, ofd->mMustExistInDir);

                     EnableWindow(hOK, Platform::isFile(checkPath));
                  }
                  break;
            }
         }
         break;
   }
   return 0;
}

//-----------------------------------------------------------------------------
// FileDialog Implementation
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(FileDialog);

FileDialog::FileDialog() : mData()
{
   // Default to File Must Exist Open Dialog style
   mData.mStyle = FileDialogData::FDS_OPEN | FileDialogData::FDS_MUSTEXIST;
   mChangePath = false;
}

FileDialog::~FileDialog()
{
}

void FileDialog::initPersistFields()
{
   addProtectedField("DefaultPath", TypeString, Offset(mData.mDefaultPath, FileDialog), &setDefaultPath, &defaultProtectedGetFn, "Default Path when Dialog is shown");
   addProtectedField("DefaultFile", TypeString, Offset(mData.mDefaultFile, FileDialog), &setDefaultFile, &defaultProtectedGetFn, "Default File when Dialog is shown");
   addProtectedField("FileName", TypeString, Offset(mData.mFile, FileDialog), &setFile, &defaultProtectedGetFn, "Default File when Dialog is shown");
   addProtectedField("Filters", TypeString, Offset(mData.mFilters, FileDialog), &setFilters, &defaultProtectedGetFn, "Default File when Dialog is shown");
   addField("Title", TypeString, Offset(mData.mTitle, FileDialog), "Default File when Dialog is shown");
   addProtectedField("ChangePath", TypeBool, Offset(mChangePath, FileDialog), &setChangePath, &getChangePath, "True/False whether to set the working directory to the directory returned by the dialog" );
   Parent::initPersistFields();
}

static const U32 convertUTF16toUTF8DoubleNULL( const UTF16 *unistring, UTF8  *outbuffer, U32 len)
{
   AssertFatal(len >= 1, "Buffer for unicode conversion must be large enough to hold at least the null terminator.");
   PROFILE_START(convertUTF16toUTF8DoubleNULL);
   U32 walked, nCodeunits, codeunitLen;
   UTF32 middleman;

   nCodeunits=0;
   while( ! (*unistring == '\0' && *(unistring + 1) == '\0') && nCodeunits + 3 < len )
   {
      walked = 1;
      middleman  = oneUTF16toUTF32(unistring,&walked);
      codeunitLen = oneUTF32toUTF8(middleman, &outbuffer[nCodeunits]);
      unistring += walked;
      nCodeunits += codeunitLen;
   }

   nCodeunits = getMin(nCodeunits,len - 1);
   outbuffer[nCodeunits] = '\0';
   outbuffer[nCodeunits+1] = '\0';

   PROFILE_END(convertUTF16toUTF8DoubleNULL);
   return nCodeunits;
}

//
// Execute Method
//
bool FileDialog::Execute()
{
   static char pszResult[MAX_PATH];
#ifdef UNICODE
   UTF16 pszFile[MAX_PATH];
   UTF16 pszInitialDir[MAX_PATH];
   UTF16 pszTitle[MAX_PATH];
   UTF16 pszFilter[1024];
   UTF16 pszFileTitle[MAX_PATH];
   // Convert parameters to UTF16*'s
   convertUTF8toUTF16((UTF8 *)mData.mDefaultFile, pszFile, sizeof(pszFile));
   convertUTF8toUTF16((UTF8 *)mData.mDefaultPath, pszInitialDir, sizeof(pszInitialDir));
   convertUTF8toUTF16((UTF8 *)mData.mTitle, pszTitle, sizeof(pszTitle));
   convertUTF8toUTF16((UTF8 *)mData.mFilters, pszFilter, sizeof(pszFilter) );
#else
   // Not Unicode, All char*'s!
   char pszFile[MAX_PATH];
   char pszFilter[1024];
   char pszFileTitle[MAX_PATH];
   dStrcpy( pszFile, sizeof(pszFile), mData.mDefaultFile );
   dStrcpy( pszFilter, sizeof(pszFilter), mData.mFilters );
   const char* pszInitialDir = mData.mDefaultPath;
   const char* pszTitle = mData.mTitle;
   
#endif

   pszFileTitle[0] = 0;

   // Convert Filters
   U8 filterLen = dStrlen( pszFilter );
   for( S32 i = 0; i < filterLen; i++ )
   {
      if( pszFilter[i] == '|' )
         pszFilter[i] = '\0';
   }
   // Add second NULL terminator at the end
   pszFilter[ filterLen + 1 ] = '\0';

   OPENFILENAME ofn;
   dMemset(&ofn, 0, sizeof(ofn));
   ofn.lStructSize      = sizeof(ofn);
   ofn.hwndOwner        = winState.appWindow;
   ofn.lpstrFile        = pszFile;


   if( !dStrncmp( mData.mDefaultFile, "", 1 ) )
      ofn.lpstrFile[0]     = '\0';


   ofn.nMaxFile         = sizeof(pszFile);
   ofn.lpstrFilter      = pszFilter;
   ofn.nFilterIndex     = 1;
   ofn.lpstrInitialDir  = pszInitialDir;
   ofn.lCustData        = (LPARAM)this;
   ofn.lpstrFileTitle   = pszFileTitle;
   ofn.nMaxFileTitle    = sizeof(pszFileTitle);

   if( mData.mTitle != StringTable->lookup("") )
      ofn.lpstrTitle = pszTitle;

   // Build Proper Flags.
   ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY;

   if(mData.mStyle & FileDialogData::FDS_BROWSEFOLDER)
   {
      ofn.lpfnHook = FolderHookProc;
      ofn.Flags |= OFN_ENABLEHOOK;
   }
   
   if( !(mData.mStyle & FileDialogData::FDS_CHANGEPATH) )
      ofn.Flags |= OFN_NOCHANGEDIR;

   if( mData.mStyle & FileDialogData::FDS_MUSTEXIST )
      ofn.Flags |= OFN_FILEMUSTEXIST;

   if( mData.mStyle & FileDialogData::FDS_OPEN && mData.mStyle & FileDialogData::FDS_MULTIPLEFILES )
      ofn.Flags |= OFN_ALLOWMULTISELECT;

   if( mData.mStyle & FileDialogData::FDS_OVERWRITEPROMPT )
      ofn.Flags |= OFN_OVERWRITEPROMPT;


   // Flag we're showing file browser so we can do some render hacking
   winState.renderThreadBlocked = true;

   // Get the current working directory, so we can back up to it once Windows has
   // done its crazyness and messed with it.
   StringTableEntry cwd = Platform::getCurrentDirectory();

   // Execute Dialog (Blocking Call)
   bool dialogSuccess = false;
   if( mData.mStyle & FileDialogData::FDS_OPEN )
      dialogSuccess = GetOpenFileName(&ofn);
   else if( mData.mStyle & FileDialogData::FDS_SAVE )
      dialogSuccess = GetSaveFileName(&ofn);

   // Dialog is gone.
   winState.renderThreadBlocked = false;

   // Restore the working directory.
   Platform::setCurrentDirectory( cwd );

   // Did we select a file?
   if( !dialogSuccess )
      return false;

   // Handle Result Properly for Unicode as well as ANSI
#ifdef UNICODE
   if(pszFileTitle[0] || ! ( mData.mStyle & FileDialogData::FDS_OPEN && mData.mStyle & FileDialogData::FDS_MULTIPLEFILES ))
      convertUTF16toUTF8( (UTF16*)pszFile, (UTF8*)pszResult, sizeof(pszResult));
   else
      convertUTF16toUTF8DoubleNULL( (UTF16*)pszFile, (UTF8*)pszResult, sizeof(pszResult));
#else
   if(pszFileTitle[0] || ! ( mData.mStyle & FileDialogData::FDS_OPEN && mData.mStyle & FileDialogData::FDS_MULTIPLEFILES ))
      dStrcpy(pszResult, sizeof(pszResult), pszFile);
   else
   {
      // [tom, 1/4/2007] pszResult is a double-NULL terminated, NULL separated list in this case so we can't just dSstrcpy()
      char *sptr = pszFile, *dptr = pszResult;
      while(! (*sptr == 0 && *(sptr+1) == 0))
         *dptr++ = *sptr++;
      *dptr++ = 0;
   }
#endif

   forwardslash(pszResult);

   // [tom, 1/5/2007] Windows is ridiculously dumb. If you select a single file in a multiple
   // select file dialog then it will return the file the same way as it would in a single
   // select dialog. The only difference is pszFileTitle is empty if multiple files
   // are selected.

   // Store the result on our object
   if( mData.mStyle & FileDialogData::FDS_BROWSEFOLDER || ( pszFileTitle[0] && ! ( mData.mStyle & FileDialogData::FDS_OPEN && mData.mStyle & FileDialogData::FDS_MULTIPLEFILES )))
   {
      // Single file selection, do it the easy way
      mData.mFile = StringTable->insert( pszResult );
   }
   else if(pszFileTitle[0] && ( mData.mStyle & FileDialogData::FDS_OPEN && mData.mStyle & FileDialogData::FDS_MULTIPLEFILES ))
   {
      // Single file selection in a multiple file selection dialog
      setDataField(StringTable->insert("files"), "0", pszResult);
      setDataField(StringTable->insert("fileCount"), NULL, "1");
   }
   else
   {
      // Multiple file selection, break out into an array
      S32 numFiles = 0;
      const char *dir = pszResult;
      const char *file = dir + dStrlen(dir) + 1;
      char buffer[1024];

      while(*file)
      {
         Platform::makeFullPathName(file, buffer, sizeof(buffer), dir);
         setDataField(StringTable->insert("files"), Con::getIntArg(numFiles++), buffer);

         file = file + dStrlen(file) + 1;
      }

      setDataField(StringTable->insert("fileCount"), NULL, Con::getIntArg(numFiles));
   }

   // Return success.
   return true;

}

ConsoleMethod( FileDialog, Execute, bool, 2, 2, "%fileDialog.Execute();" )
{
   return object->Execute();
}

//-----------------------------------------------------------------------------
// Dialog Filters
//-----------------------------------------------------------------------------
bool FileDialog::setFilters(void* obj, const char* data)
{
   // Will do validate on write at some point.
   if( !data )
      return true;

   return true;

};


//-----------------------------------------------------------------------------
// Default Path Property - String Validated on Write
//-----------------------------------------------------------------------------
bool FileDialog::setDefaultPath(void* obj, const char* data)
{

   if( !data || !dStrncmp( data, "", 1 ) )
      return true;

   // Copy and Backslash the path (Windows dialogs are VERY picky about this format)
   static char szPathValidate[512];
   dStrcpy( szPathValidate, sizeof(szPathValidate), data );

   Platform::makeFullPathName( data,szPathValidate, sizeof(szPathValidate));
   backslash( szPathValidate );

   // Remove any trailing \'s
   S8 validateLen = dStrlen( szPathValidate );
   if( szPathValidate[ validateLen - 1 ] == '\\' )
      szPathValidate[ validateLen - 1 ] = '\0';

   // Now check 
   ResourceManager->addPath(szPathValidate, true);
   if( Platform::isDirectory( szPathValidate ) )
   {
      // Finally, assign in proper format.
      FileDialog *pDlg = static_cast<FileDialog*>( obj );
      pDlg->mData.mDefaultPath = StringTable->insert( szPathValidate );
   }
#ifdef POWER_DEBUG
   else
      Con::errorf(ConsoleLogEntry::GUI, "FileDialog - Invalid Default Path Specified!");
#endif

   return false;

};

//-----------------------------------------------------------------------------
// Default File Property - String Validated on Write
//-----------------------------------------------------------------------------
bool FileDialog::setDefaultFile(void* obj, const char* data)
{
   if( !data || !dStrncmp( data, "", 1 ) )
      return true;

   // Copy and Backslash the path (Windows dialogs are VERY picky about this format)
   static char szPathValidate[512];
   Platform::makeFullPathName( data,szPathValidate, sizeof(szPathValidate) );
   backslash( szPathValidate );

   // Remove any trailing \'s
   S8 validateLen = dStrlen( szPathValidate );
   if( szPathValidate[ validateLen - 1 ] == '\\' )
      szPathValidate[ validateLen - 1 ] = '\0';

   // Finally, assign in proper format.
   FileDialog *pDlg = static_cast<FileDialog*>( obj );
   pDlg->mData.mDefaultFile = StringTable->insert( szPathValidate );

   return false;
};

//-----------------------------------------------------------------------------
// ChangePath Property - Change working path on successful file selection
//-----------------------------------------------------------------------------
bool FileDialog::setChangePath(void* obj, const char* data)
{
   bool bMustExist = dAtob( data );

   FileDialog *pDlg = static_cast<FileDialog*>( obj );

   if( bMustExist )
      pDlg->mData.mStyle |= FileDialogData::FDS_CHANGEPATH;
   else
      pDlg->mData.mStyle &= ~FileDialogData::FDS_CHANGEPATH;

   return true;
};

const char* FileDialog::getChangePath(void* obj, const char* data)
{
   FileDialog *pDlg = static_cast<FileDialog*>( obj );
   if( pDlg->mData.mStyle & FileDialogData::FDS_CHANGEPATH )
      return StringTable->insert("true");
   else
      return StringTable->insert("false");
}

bool FileDialog::setFile(void* obj, const char* data)
{
   return false;
};

//-----------------------------------------------------------------------------
// OpenFileDialog Implementation
//-----------------------------------------------------------------------------
OpenFileDialog::OpenFileDialog()
{
   // Default File Must Exist
   mData.mStyle = FileDialogData::FDS_OPEN | FileDialogData::FDS_MUSTEXIST;
}

OpenFileDialog::~OpenFileDialog()
{
   mMustExist = true;
   mMultipleFiles = false;
}

IMPLEMENT_CONOBJECT(OpenFileDialog);

//-----------------------------------------------------------------------------
// Console Properties
//-----------------------------------------------------------------------------
void OpenFileDialog::initPersistFields()
{
   addProtectedField("MustExist", TypeBool, Offset(mMustExist, OpenFileDialog), &setMustExist, &getMustExist, "True/False whether the file returned must exist or not" );
   addProtectedField("MultipleFiles", TypeBool, Offset(mMultipleFiles, OpenFileDialog), &setMultipleFiles, &getMultipleFiles, "True/False whether multiple files may be selected and returned or not" );
   
   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// File Must Exist - Boolean
//-----------------------------------------------------------------------------
bool OpenFileDialog::setMustExist(void* obj, const char* data)
{
   bool bMustExist = dAtob( data );

   OpenFileDialog *pDlg = static_cast<OpenFileDialog*>( obj );
   
   if( bMustExist )
      pDlg->mData.mStyle |= FileDialogData::FDS_MUSTEXIST;
   else
      pDlg->mData.mStyle &= ~FileDialogData::FDS_MUSTEXIST;

   return true;
};

const char* OpenFileDialog::getMustExist(void* obj, const char* data)
{
   OpenFileDialog *pDlg = static_cast<OpenFileDialog*>( obj );
   if( pDlg->mData.mStyle & FileDialogData::FDS_MUSTEXIST )
      return StringTable->insert("true");
   else
      return StringTable->insert("false");
}

//-----------------------------------------------------------------------------
// Can Select Multiple Files - Boolean
//-----------------------------------------------------------------------------
bool OpenFileDialog::setMultipleFiles(void* obj, const char* data)
{
   bool bMustExist = dAtob( data );

   OpenFileDialog *pDlg = static_cast<OpenFileDialog*>( obj );

   if( bMustExist )
      pDlg->mData.mStyle |= FileDialogData::FDS_MULTIPLEFILES;
   else
      pDlg->mData.mStyle &= ~FileDialogData::FDS_MULTIPLEFILES;

   return true;
};

const char* OpenFileDialog::getMultipleFiles(void* obj, const char* data)
{
   OpenFileDialog *pDlg = static_cast<OpenFileDialog*>( obj );
   if( pDlg->mData.mStyle & FileDialogData::FDS_MULTIPLEFILES )
      return StringTable->insert("true");
   else
      return StringTable->insert("false");
}

//-----------------------------------------------------------------------------
// SaveFileDialog Implementation
//-----------------------------------------------------------------------------
SaveFileDialog::SaveFileDialog()
{
   // Default File Must Exist
   mData.mStyle = FileDialogData::FDS_SAVE | FileDialogData::FDS_OVERWRITEPROMPT;
   mOverwritePrompt = true;
}

SaveFileDialog::~SaveFileDialog()
{
}

IMPLEMENT_CONOBJECT(SaveFileDialog);

//-----------------------------------------------------------------------------
// Console Properties
//-----------------------------------------------------------------------------
void SaveFileDialog::initPersistFields()
{
   addProtectedField("OverwritePrompt", TypeBool, Offset(mOverwritePrompt, SaveFileDialog), &setOverwritePrompt, &getOverwritePrompt, "True/False whether the dialog should prompt before accepting an existing file name" );
   
   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// Prompt on Overwrite - Boolean
//-----------------------------------------------------------------------------
bool SaveFileDialog::setOverwritePrompt(void* obj, const char* data)
{
   bool bMustExist = dAtob( data );

   SaveFileDialog *pDlg = static_cast<SaveFileDialog*>( obj );

   if( bMustExist )
      pDlg->mData.mStyle |= FileDialogData::FDS_OVERWRITEPROMPT;
   else
      pDlg->mData.mStyle &= ~FileDialogData::FDS_OVERWRITEPROMPT;

   return true;
};

const char* SaveFileDialog::getOverwritePrompt(void* obj, const char* data)
{
   SaveFileDialog *pDlg = static_cast<SaveFileDialog*>( obj );
   if( pDlg->mData.mStyle & FileDialogData::FDS_OVERWRITEPROMPT )
      return StringTable->insert("true");
   else
      return StringTable->insert("false");
}

//-----------------------------------------------------------------------------
// OpenFolderDialog Implementation
//-----------------------------------------------------------------------------

OpenFolderDialog::OpenFolderDialog()
{
   mData.mStyle = FileDialogData::FDS_OPEN | FileDialogData::FDS_OVERWRITEPROMPT | FileDialogData::FDS_BROWSEFOLDER;

   mMustExistInDir = "";
}

IMPLEMENT_CONOBJECT(OpenFolderDialog);

void OpenFolderDialog::initPersistFields()
{
   addField("fileMustExist", TypeFilename, Offset(mMustExistInDir, OpenFolderDialog), "File that must in selected folder for it to be valid");

   Parent::initPersistFields();
}
