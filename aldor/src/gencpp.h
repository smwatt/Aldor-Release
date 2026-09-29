#ifndef _EMITCPP_H_
#define _EMITCPP_H_


/* ---------- ENUMERALS ---------- */

typedef enum { AC_NONE, AC_FUNCTION, AC_CLASS, AC_TEMPL_CLASS,
                 AC_ABS_CLASS, AC_ABS_TMPL_CLASS, AC_VAR, AC_CST } cpp_types;

typedef enum { CT_FUNC, CT_VAR, CT_STR, CT_PARENT, CT_TMPLPARMS, CT_EXTRA } CellType;

typedef enum { NONE, T4AT_BT, T4AT_UT } Type4AldorType;

/* --------- MACROS --------------- */

#define alloc(x) (x *) stoAlloc(OB_Other,sizeof(x))
/* #define STRCAT strcat */
#define STRSIZE 500
#define InitStr() strAlloc(STRSIZE)
#define tfIsMultiReturn(tf) (tfIsMulti(tf) && tfMultiArgc(tf))

#define BANG  '!'
#define QMARK '?'

/* ---------- GLOBAL VARIABLES ------------ */

extern String CurrentClassAldor;
extern String CurrentClassCpp;

extern MutString CurrentClass;
extern MutString CurrentClassInUse;
extern const int theTRUE;
extern const int theFALSE;
extern String *basicAldorTypes;
extern int bAT_size;
extern String *basicCppTypes;
extern String commonOperators[];
extern int CO_size;
extern String infixOps[];
extern int IO_size;
extern String typesToSkip[];
extern int tTS_size;
extern String cppKeywords[];
extern int CK_size;
#if 0
#if EDIT_1_0_n2_07
extern struct ccSpecCharId_info *ccSpecCharIdTable;
#else
extern struct ccSpecCharId_info ccSpecCharIdTable[];
#endif
#endif
extern int globalFuncCounter;
extern const int TabZero;


/* ---------- TO ACCESS VALUES OF ABSYN (extension of what is in absyn.h) --------- */

#define abIdStr(a)       ((a)->abId.sym.string())
#define abDefineLhs(a)   ((a)->abDefine.lhs)
#define abDefineRhs(a)   ((a)->abDefine.rhs)
#define abDeclareId(a)   ((a)->abDeclare.id)
#define abDeclareType(a) ((a)->abDeclare.type)
#define abSeqArg(a,i)    ((a)->abSequence.argv[(i)])
#define abCommaArg(a,i)  ((a)->abComma.argv[(i)])
#define abWithin(a)      ((a)->abWith.within)
#define abWithBase(a)    ((a)->abWith.base)
#define abAddBase(a)     ((a)->abAdd.base)

/* ---------- TRUE/FALSE values ---------- */

#define BL_Exported       theTRUE
#define BL_UsePercent     theTRUE
#define BL_WithType       theTRUE
#define BL_WithName       theTRUE
#define BL_InBody         theTRUE
#define BL_Global         theTRUE
#define BL_IsStatic       theTRUE
#define BL_Abstract       theTRUE
#define BL_Export         theTRUE
#define BL_IsTmpl         theTRUE
#define BL_UseTmplParms   theTRUE
#define BL_ForDeclInBody  theTRUE

/* ---------- DATA STRUCTURES ---------- */

typedef enum { OPERATOR, SPECIAL_SYM, REGULAR } OpState;

/* ------------- */

typedef struct cell {
  struct cell *next;
  CellType    ct;
  void        *Item;
} Cell;

typedef struct {
  Cell *head;
  Cell *current;
  Cell *tail;
} MyList;

/* ------------- */

typedef struct {
  /* example for template <class T, class U> class A {...}; */
  String basic;  /* A: borrowed/read-only name */
  String export_; /* ATU: immutable after construction */
  String typeCpp; /* A<PercentType,T,U> */
  String typeAldor; /* A(T,U) */
} Ident;

typedef struct _class_ {
  Ident     id;
  int       methCounter;
  int       classCounter;
  MyList      *extraClasses;
  TForm     params;
  TForm     methods;
  MyList      *parents;
} Class;

typedef struct {
  MutString    id;
  TForm     mapType;      /* from there we can get everything: params and return type */
  Class     *ParentClass; /* needed to translate '%' */
  MyList      *params;
  AbSyn     ret_typ;
  int       isOperator;
  OpState   OpStatus;
  int       isStatic;
  int       multipleReturns;
} Function;

typedef struct {
  Ident     id;
  AbSyn     type;
} Parent;

/* ---------- TO ACCESS VALUES OF THE DATA STRUCTS ----------- */

#define atBt(a)    ((a)->basetype)    /* Basetype from an Aldor Type */
#define funcRet(f) ((f)->ret_typ)       /* Return type of a function   */

/*
 * ---------------------------------------------------------
 *              FUNCTIONS OF THE INTEROP
 * ---------------------------------------------------------
 */

/*
 * MyList functions
 * ================
 *
 * Implements a List ADT.
 * Will be replaced by the "standard" list of the aldor compiler code
 */

MyList *InitMyList(void);
void Append(MyList *, CellType, void *);
void Rewind(MyList *);
void *GetItem(MyList *);
void GotoNext(MyList *);
void HeadToNext(MyList *);
int  Empty(MyList *);
int  NotEmpty(MyList *);
int  EOList(MyList *);

Function *InitFunc(void);
Class *InitClass(void);

MutString GettypId(AbSyn);
cpp_types GettypApply(AbSyn);
cpp_types GettypType(AbSyn);
cpp_types GettypDeclare(AbSyn);
cpp_types GettypDefine(AbSyn);

void FreeList(MyList *);
void FreeFunction(Function *);
void FreeClass(Class *);

MutString MappingTypes(String);
void genCpp(AbSyn, MutString, MutString);
MutString STRCAT(MutString, String);


int cppOption(MutString);
void updateBasicTypes(MutString, int);
int InitBasicTypesArray(String);
void CreateOutputFiles(MutString, MutString, FILE **, FILE **);
int SequenceCheck(AbSyn);
void ExportGen(AbSyn, FILE *, FILE *);
void CodeGen(AbSyn, FILE *, FILE *);
MutString outputCppFnPtrParms(TForm, Class *, int, int, int, int, String);
MutString GenCppFunc_ParamsRetFnPtr(TForm, Class *, int, int, int);
MutString GenCppFunc_RetFnPtr(TForm);

/* ---------- Header files ---------- */

/* ----- Building  */

/* ---------- ERROR ---------- */
Class *errorBuildClass(Class *);

/* ---------- ID HANDLING ---------- */
void BuildParentIdAbs(Ident *, AbSyn);
void BuildExtraClassId(Class *, Class *, int);
void BuildClassId(Class *, int);
MutString BldId(AbSyn);
void BuildParentId(Ident *, AbSyn);

/* ---------- ABSTRACT CLASS HANDLING ---------- */
void getParentsAbs(Class *, TForm, int);
Class *BuildAbstractClass(AbSyn, int);

/* ---------- EXTRA CLASS HANDLING ---------- */
Class *buildExtraClass(Class *, TForm, int);

/* ---------- CLASS HANDLING ---------- */
void getParents(Class *, TForm, int);
Class *BuildClass(AbSyn, int);

/* ----- Determination of type */

MutString GettypId(AbSyn);
cpp_types GettypApply(AbSyn);
cpp_types GettypType(AbSyn);
cpp_types GettypDeclare(AbSyn);
cpp_types GettypDefine(AbSyn);
TForm tfFollowDefDecl(TForm);


/* ----- Main functions for code generation */

/* ---------- FUNCTION SIGNATURES ---------- */

MutString GenAldorFnSig(TForm, String, Class *, int, int, int, int);
MutString GenCppFnSig(TForm, String, Class *, int, int);

/* ---------- END FUNCTION SIGNATURES ---------- */

/* ---------- STUBS ---------- */

MutString GenAldorStubs(TForm, String, Class *, int, int);

/* ---------- END STUBS ---------- */

/* ---------- GLOBAL FUNCTIONS ---------- */

MutString GenCppFunc(TForm, String, Class *, int);
void GenFunction(AbSyn, FILE *, FILE *);

/* ---------- END GLOBAL FUNCTIONS ---------- */

/* ---------- HEADER ---------- */

void GenCppConstructors(Class *, int, FILE *);
void GenCppDestructor(String, int, FILE *);
void GenCppAccessToRealObj(Class *, int, FILE *);
void GenCppGiveType(Class *, int, FILE *);
void GenHeaderForClass(Class *, FILE *);

/* ---------- END HEADER ---------- */

/* ---------- CLASS ---------- */

MutString GCFC_HeaderTmpl(Class *);
MutString GCFC_HeaderInher(Class *);
MutString GCFC_Methods(Class *, Syme);
void GenExtraClass(Class *, Class *, FILE *);
void GenClassesForDomains(AbSyn, int, FILE *, FILE *);
void GenClassForCategories(AbSyn, int, FILE *, FILE *);

/* ---------- END CLASS ---------- */

/* ---------- EXPORT / IMPORT ---------- */

void GenExportClass(AbSyn, int, FILE *, FILE *);
void GenExportFunction(AbSyn, FILE *, FILE *);

/* ---------- END EXPORT / IMPORT ---------- */

/* ----- Generation of methods */

/* ---------- METHODS ---------- */

MutString GenCppBodyForMethods(TForm, String, Class *);
MutString GenCppBodyStaticPercentForMethods(Syme, Class *, int, int);
void GenMethodsForClass(Class *, FILE *);

/* ---------- END METHODS ---------- */


/* ----- Special work for multiple return functions */

/* ---------- MULTIPLE RETURN ---------- */

MutString GenCppMultiRetLocalDecl(TForm, int);
MutString GenCppMultiRetAssign(TForm);
MutString GenCppMultiRet(TForm, int, int, int, int);

/* ---------- END MULTIPLE RETURN ---------- */


/* ----- Generation of parameters for methods */

/* ---------- PARAMS HANDLING ---------- */

/* -- Common Code for Aldor -- */
MutString outputAldorTmplParms(TForm, int);
MutString outputAldorRegParms(TForm, int, int, int, String, MutString);

/* -- Aldor Params -- */
MutString GenAldorParams(TForm, Class *, int, int, int, int, int);
MutString GenAldorOneParam(TForm, int, int, int, int);

/* -- Common Code for C++ -- */

/* -- C++ Params -- */
MutString GenCppParams(TForm, Class *, int, int, int, int, int);
MutString GenCppParamsBody_VirtualForStatic(TForm, Class *, int);
MutString GenCppParamsStaticPercent(TForm, Class *, int, int, int, int);
MutString GenCppParams_Body(TForm, String, Class *, int);
MutString GenCppParams_Global_Body(TForm, String);

/* ---------- END PARAMS ---------- */

/* ----- Type handling in code generation */

/* ---------- TYPES ---------- */

/* -- Basic -- */
MutString MappingTypes(String);
MutString MachineToCpp(AbSyn);

/* -- AbSyn -- */
MutString ApplyToAldor(AbSyn, int);
MutString CommaToAldor(AbSyn, int);
MutString QualifyToString(AbSyn, int);
MutString AbSynToAldor(AbSyn, int);
MutString AbSynToCpp(AbSyn);
int AbSynIsUserDef(AbSyn);
int AbSynIsVoid(AbSyn);

/* -- TForm -- */
MutString TFormToAldor(TForm, int);
TForm tfMakeSubst(TForm);
TForm tfFollowDefDecl(TForm);

/* -- Import/Export -- */
MutString GiveTypeAldorExport(Class *, int);
MutString GiveTypeCppExtern(Class *, int);
void GenGiveTypeForExport(Class *, FILE *, FILE *);

/* ---------- END TYPES ---------- */


/* ----- Utilities for code generation */

/* ---------- UTILS ---------- */

/* -- MutString -- */
void printTab(FILE *, int);
MutString sprintTab(MutString, int);
/* LDR */
#if 0
#pragma warning (disable:4030)
MutString itoa(int);
#pragma warning (default:4030)
#endif
MutString OutputDomNameAldor(String);
MutString addUnderScore(String);
MutString transOperatorLike(String);
MutString AldorifySpeSym(String);
MutString RealTypeString(AbSyn, int);

/* -- Test -- */
int PercentsInParms(TForm, Class *);
int StaticOrNot(TForm, Class *);
int SpecialCppFunc(String);

/* -- ID Handling -- */
MutString operatorForC(String);
MutString createFnIdExternC(String, Class *);
MutString createFnIdExportC(String, Class *);
MutString createFnIdForCPP(String, Class *);
MutString GenAldorFuncName(String, Class *, int);
String GetIdFromAbSyn(AbSyn);
int abIsFuncPtr(AbSyn);
int tfIsFuncPtr(TForm);
int IsCommonOperator(String);
int IsInfixOperator(String);
int IsSpecialSymbol(char,int);
String GetStringForSpecialSymbol(char,int);
int ToSkipClass(Class*);


/* ---------- END UTILS ---------- */

/* ----- Init of structures used for C++ gen */

Function *InitFunc(void);
Class *InitClass(void);

/* ----- Type List */

MyList *InitList(void);
void Append(MyList *, CellType, void *);
void Rewind(MyList *);
void *GetItem(MyList *);
void GotoNext(MyList *);
void HeadToNext(MyList *);
int Empty(MyList *);
int NotEmpty(MyList *);
int EOList(MyList *);

/* ----- Memory management */

void FreeList(MyList *);
void FreeFunction(Function *);
void FreeClass(Class *);


#endif
