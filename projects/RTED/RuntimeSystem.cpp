/*
 * RuntimeSystem.cpp
 *
 *  Created on: Jul 2, 2009
 *      Author: panas2
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

//#include "RuntimeSystem.h"
#include "CppRuntimeSystem/CppRuntimeSystem.h"
#include "rted_qt/rted_qt.h"



/*********************************************************
 * Constructor
 ********************************************************/
void
RuntimeSystem_Const_RuntimeSystem() {
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs->setOutputFile("result.txt");
}

/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void
RuntimeSystem_roseRtedClose() {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->doProgramExitChecks();
}







// ***************************************** ARRAY FUNCTIONS *************************************





/*********************************************************
 * This function is called when an array is created
 * name      : variable name
 * manglname : variable mangl_name
 * type      : Sage Type
 * dimension : 1 or 2
 * sizeA     : size of dimension 1
 * sizeB     : size of dimension 2
 * ismalloc  : Allocated through malloc?
 * filename  : file location
 * line      : linenumber
 ********************************************************/
void
RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name, int dimension,  
			      unsigned long int address, long int sizeA, long int sizeB,
			      int ismalloc, const char* filename, const char* line, const char* lineTransformed){
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	if (dimension==1)
	  rs->createMemory(address, sizeA);
	else
	  rs->createMemory(address, sizeA*sizeB);

}

/*********************************************************
 * This function is called when an array is accessed
 * name      : variable name
 * posA      : array[posA][]
 * posB      : array[][posB]
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void
RuntimeSystem_roseArrayAccess(const char* name, int posA, int posB, const char* filename,
			      unsigned long int address, long int size, 
			      const char* line, const char* lineTransformed, const char* stmtStr){
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint(SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	//fixme: we do not know yet wheather array access
	//is a read or write
	bool read = true;
	if (read)
	  rs->checkMemRead(address,size);
}

// ***************************************** ARRAY FUNCTIONS *************************************








// ***************************************** FUNCTION CALL *************************************
/*********************************************************
 * Check if a function call is interesting, i.e. contains a
 * call to a function that we need to check the parameters of
 ********************************************************/
int
RuntimeSystem_isInterestingFunctionCall(const char* name) {
  int interesting=0;//false;
  if ( ( strcmp(name,"memcpy")==0 ||
	 strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strcpy")==0 ||
	 strcmp(name ,"strncpy")==0 ||
	 strcmp(name ,"strcat")==0 ||
	 strcmp(name ,"strncat")==0 ||
	 strcmp(name ,"strlen")==0 ||
	 strcmp(name ,"strchr")==0 ||
	 strcmp(name ,"strpbrk")==0 ||
	 strcmp(name ,"strspn")==0  ||
	 strcmp(name ,"strstr")==0
	 )) {
    interesting=1;
  }
  return interesting;
}

/*********************************************************
 * Check if the function that is called is modifying
 * i.e. whether it changes memory or just accesses it.
 * If it accesses memory, we need to ensure that the
 * memory is NULL terminated if const char*
 * we assume that parameter 2 than has the \0 token
 ********************************************************/
int
RuntimeSystem_isModifyingOp(const char* name) {
  int modifying=0;//false;
  // we do not need the actualLength of the
  // first parameter for the following
  if ( ( strcmp(name,"memcpy")==0 ||
	 strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strcpy")==0 ||
	 strcmp(name ,"strncpy")==0
	 )) {
    modifying=1;
  }
  return modifying;
}

/*********************************************************
 * Return the number of parameters for a specific function call
 ********************************************************/
int
RuntimeSystem_getParamtersForFuncCall(const char* name) {
  int dim=0;
  if ( ( strcmp(name,"memcpy")==0 ||
	 strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strncat")==0 )) {
    dim=3;
  } else if ((
	      strcmp(name ,"strncpy")==0 ||
	      strcmp(name ,"strcpy")==0 ||
	      strcmp(name ,"strchr")==0 ||
	      strcmp(name ,"strpbrk")==0 ||
	      strcmp(name ,"strspn")==0  ||
	      strcmp(name ,"strstr")==0  ||
	      strcmp(name ,"strcat")==0 )) {
    dim=2;
  } else if ((
	      strcmp(name ,"strlen")==0
	      )) {
    dim=1;
  }
  return dim;
}

/*********************************************************
 * Check if a function call is a call to a function
 * on our ignore list. We do not want to check those
 * functions right now.
 * This check makes sure that we dont push variables
 * on the stack for functions that we dont check
 * and hence the generated code is cleaner
 ********************************************************/
int
RuntimeSystem_isFileIOFunctionCall(const char* name) {
  int interesting=0;//false;
  if ( ( strcmp(name,"fopen")==0 ||
	 strcmp(name ,"fgetc")==0 ||
	 strcmp(name ,"fputc")==0 ||
	 strcmp(name ,"fclose")==0
	 )) {
    interesting=1;
  }
  return interesting;
}



/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * memcpy, memmove, strcpy, strncpy, strcat, strncat
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable),
 *           :          number of const char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void
RuntimeSystem_handleSpecialFunctionCalls(const char* fname,const char** args, int argsSize,
					 const char* filename, const char* line,
					 const char* lineTransformed,
					 const char* stmtStr, const char* leftHandSideVar) {
  // unhandled
}





/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * fopen, fgetc
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable),
 *           :          number of const char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void
RuntimeSystem_handleIOFunctionCall(const char* fname,const char** args,
				   int argsSize, const char* filename, const char* line, const char* lineTransformed,
				   const char* stmtStr, const char* leftHandSideVar) {
  // unhandled
}





/*********************************************************
 * This function is called when a function is called
 * The number of parameters is variable but some of the are fixed:
 * count        : number of parameters
 * name         : variable name if it is a variable to be put on the stack
 *              : function name if it is a function that is being called
 * mangled_name : mangled name of the above
 * scope        : Scope indicator (might not be necessary)
 * insertBefore : Indicates whether to push or pop a variable form the stack
 * filename     : file location
 * line         : linenumber
 * stmtStr      : unparsed version of the line to be used for error message
 * ...          : variable amount of additional parameters
 ********************************************************/
void
RuntimeSystem_roseFunctionCall(int count, ...) {
  // handle the parameters within this call
  va_list vl;
  va_start(vl,count);
  const char** args = (const char**)malloc(sizeof(const char*)*count+1);
  int posArgs=0;
  const char* name = NULL;
  const char* filename = NULL;
  const char* line=NULL;
  const char* lineTransf=NULL;
  const char* stmtStr=NULL;
  const char* leftVar=NULL;
  //cerr << "arguments : " <<  count << endl;
  int i=0;
  for ( i=0;i<count;i++)    {
    const char* val=  va_arg(vl,const char*);
    //    if (val)
    //  printMessage("  %d      val : '%s' ---",i,val);
    const char *iter2=NULL;
    int size =0;
    // for ( iter2 = val; *iter2 != '\0'; ++iter2) {
    //  printMessage("%c",*iter2); size++;
    //} printMessage("--- size : %d \n",size);

    if (i==0) name = val;
    else if (i==1) filename =  val;
    else if (i==2) line = val;
    else if (i==3) lineTransf = val;
    else if (i==4) stmtStr = val;
    else if (i==5) leftVar = val;
    else {
      args[posArgs++]=val;
    }
  }
  va_end(vl);


  if (RuntimeSystem_isInterestingFunctionCall(name)==1) {
    // if the string name is one of the above, we handle it specially
    RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar);
  } else if (RuntimeSystem_isFileIOFunctionCall(name)==1) {
    RuntimeSystem_handleIOFunctionCall(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar);
  } else {
    //printMessage("Unknown Function call to RuntimeSystem!\n");
    exit(1);
  }
}



// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************
// TODO 1 djh: check for missing frees

void RuntimeSystem_roseEnterScope(const char* name) {
	RuntimeSystem * rs = RuntimeSystem::instance();
    rs->beginScope(name);
}

void RuntimeSystem_roseExitScope( const char* filename, const char* line, const char* stmtStr) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->endScope();
}




// ***************************************** SCOPE HANDLING *************************************


// ***************************************** VARIABLE DECLARATIONS *************************************
/*********************************************************
 * This function tells the runtime system that a variable is created
 * we store the type of the variable and whether it has been intialized
 ********************************************************/
void RuntimeSystem_roseCreateVariable( const char* name,
				      const char* mangled_name,
				      const char* type,
              unsigned long int address,
              unsigned int size,
				      int init,
				      const char* fOpen,
				      const char* filename, const char* line,
				      const char* lineTransformed) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint( SourcePosition(filename,atoi(line), atoi(lineTransformed)));
	rs->createVariable(address,name,mangled_name,type);
}



/*********************************************************
 * For a given variable name, check if it is present
 * in the pool of variables created and return mangled_name
 ********************************************************/
void
RuntimeSystem_roseInitVariable(const char* name,
			       const char* mangled_name,
			       const char* typeOfVar2,
			       const char* baseType,
			       unsigned long long address,
			       unsigned int size2,
			       unsigned int size,
			       int ismalloc,
			       const char* filename,
			       const char* line,
			       const char* lineTransformed, const char* stmtStr) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint( SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	//fixme: the size will not work for structs yet!
	rs->checkMemWrite(address,size);
}


/*********************************************************
 * This function tells the runtime system that a variable is used
 ********************************************************/
void RuntimeSystem_roseAccessVariable( const char* name,
				       const char* mangled_name,
				       unsigned long long address, 
				       unsigned int size,
				       const char* filename, const char* line,
				       const char* lineTransformed,
				       const char* stmtStr) {
	RuntimeSystem * rs = RuntimeSystem::instance();
	rs->checkpoint( SourcePosition(filename,atoi(line),atoi(lineTransformed)));
	rs->checkMemRead(address, size);
}

// ***************************************** VARIABLE DECLARATIONS *************************************

// vim:sw=2 ts=2 et sta:
