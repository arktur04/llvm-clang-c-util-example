#include <clang-c/Index.h>
#include <clang/AST/Decl.h>
#include "clang/AST/DeclBase.h"
#include "clang/AST/Expr.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace clang;

//logging functions
std::string getLocationString(CXSourceLocation Loc) {
    CXFile File;
    unsigned Line, Column;
    clang_getFileLocation(Loc, &File, &Line, &Column, nullptr);
    CXString FileName = clang_getFileName(File);
    std::ostringstream ostr;
    ostr << clang_getCString(FileName) << ":" << Line << ":" << Column;
    clang_disposeString(FileName);
    return ostr.str();
}

void printCursor(CXCursor cursor) {
    CXString displayName = clang_getCursorDisplayName(cursor);
    std::cout << clang_getCString(displayName) << "@" << getLocationString(clang_getCursorLocation(cursor)) << "\n";
    clang_disposeString(displayName);
}
//==============
// extracted from CXCursor.cpp
const Decl *getCursorDecl(CXCursor Cursor) {
    return static_cast<const Decl *>(Cursor.data[0]);
}

const Stmt *getCursorStmt(CXCursor Cursor) {
    if (Cursor.kind == CXCursor_ObjCSuperClassRef ||
            Cursor.kind == CXCursor_ObjCProtocolRef ||
            Cursor.kind == CXCursor_ObjCClassRef)
        return nullptr;
    return static_cast<const Stmt *>(Cursor.data[1]);
}

const Expr *getCursorExpr(CXCursor Cursor) {
    return dyn_cast_or_null<Expr>(getCursorStmt(Cursor));
}
//==================
CXChildVisitResult visitor( CXCursor cursor, CXCursor /* parent */, CXClientData /*clientData*/ )
{
    CXSourceLocation location = clang_getCursorLocation( cursor );
    if( clang_Location_isFromMainFile( location ) == 0 )
        return CXChildVisit_Continue;

    CXCursorKind cursorKind = clang_getCursorKind( cursor );
    // finding local variables
    if(clang_getCursorKind(cursor) == CXCursor_VarDecl) {
        if(const VarDecl* VD = dyn_cast_or_null<const VarDecl>(getCursorDecl(cursor))) {
            if( VD->isLocalVarDecl()) {
                std::cout << "local variable: ";
                printCursor(cursor);
            }
        }
    }
    // finding referenced variables
    if(cursorKind == CXCursor_DeclRefExpr) {
        if(const DeclRefExpr* DRE = dyn_cast_or_null<const DeclRefExpr>(getCursorExpr(cursor))) {
            if(const VarDecl* VD = dyn_cast_or_null<const VarDecl>(DRE->getDecl())) {
                if(VD->isLocalVarDecl()) {
                    std::cout << "reference to local variable: ";
                    printCursor(cursor);
                }
            }
        }
    }
    // finding functions not defined in the module
    if(cursorKind == CXCursor_CallExpr) {
        if (const Expr *E = getCursorExpr(cursor)) {
            if(isa<const CallExpr>(E)) {
                CXCursor Definition = clang_getCursorDefinition(cursor);
                if (clang_equalCursors(Definition, clang_getNullCursor())) {
                    std::cout << "function is not defined here: ";
                    printCursor(cursor);
                }
            }
        }
    }
    // traverse to the children
    clang_visitChildren( cursor, visitor, nullptr );
    return CXChildVisit_Continue;
}

int main (int argc, char** argv) {
    CXIndex index = clang_createIndex (
        0, // excludeDeclarationFromPCH
        1  // displayDiagnostics
        );
    CXTranslationUnit unit = clang_parseTranslationUnit (
        index, // CIdx
        0, // source_filename
        argv, // command_line_args
        argc, // num_command_line_args
        0, // unsave_files
        0, // num_unsaved_files
        CXTranslationUnit_None // options
        );
    if (!unit) {
        std::cout << "Translation unit was not created\n";
    }
    else {
        CXCursor root = clang_getTranslationUnitCursor(unit);
        clang_visitChildren(root, visitor, nullptr);
    }
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
}
