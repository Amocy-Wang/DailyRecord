#include <string>
#include <iostream>
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/CommentVisitor.h"
#include "clang/AST/DeclLookups.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/AST/TypeVisitor.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceManager.h"
using namespace std;
using namespace clang;
using namespace llvm;
using namespace clang::tooling;

class MyVisitor : public RecursiveASTVisitor<MyVisitor>{
    public:
    explicit MyVisitor(CompilerInstance *CI)
      : Context(&(CI->getASTContext())) {}

    bool VisitFunctionDecl(FunctionDecl *D)
    {
        std::cout << "visiting func decl: " << D->getNameInfo().getAsString() << std::endl;
        return true;
    }
    private:
    ASTContext *Context;

};

class MyASTConsumer : public ASTConsumer{
    private:
    MyVisitor *Visitor;
    public:
    explicit MyASTConsumer(CompilerInstance *CI)
      :Visitor(new MyVisitor(CI)) {}
    virtual void HandleTranslationUnit(clang::ASTContext &Context){
      Visitor->TraverseDecl(Context.getTranslationUnitDecl());
    }
};

class MyFrontendAction : public ASTFrontendAction {
    public:
         virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef Infile){
             return std::unique_ptr<ASTConsumer>(new MyASTConsumer(&CI));
         }
};

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");

int main(int argc, const char **argv)
{
  /*if(argc > 1)
      return clang::tooling::runToolOnCode(new MyFrontendAction, argv[1]);*/


  CommonOptionsParser op(argc, argv, MatcherSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());




}
