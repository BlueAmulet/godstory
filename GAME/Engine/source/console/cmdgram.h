typedef union {
   char              c;
   int               i;
   const char *      s;
   char *            str;
   double            f;
   StmtNode *        stmt;
   ExprNode *        expr;
   SlotAssignNode *  slist;
   VarNode *         var;
   SlotDecl          slot;
   InternalSlotDecl  intslot;
   ObjectBlockDecl   odcl;
   ObjectDeclNode *  od;
   AssignDecl        asn;
   IfStmtNode *      ifnode;
} YYSTYPE;
#define	rwDEFINE	258
#define	rwENDDEF	259
#define	rwDECLARE	260
#define	rwBREAK	261
#define	rwELSE	262
#define	rwCONTINUE	263
#define	rwGLOBAL	264
#define	rwIF	265
#define	rwNIL	266
#define	rwRETURN	267
#define	rwWHILE	268
#define	rwDO	269
#define	rwENDIF	270
#define	rwENDWHILE	271
#define	rwENDFOR	272
#define	rwDEFAULT	273
#define	rwFOR	274
#define	rwDATABLOCK	275
#define	rwSWITCH	276
#define	rwCASE	277
#define	rwSWITCHSTR	278
#define	rwCASEOR	279
#define	rwPACKAGE	280
#define	rwNAMESPACE	281
#define	rwCLASS	282
#define	rwMESSAGE	283
#define	ILLEGAL_TOKEN	284
#define	CHRCONST	285
#define	INTCONST	286
#define	TTAG	287
#define	VAR	288
#define	IDENT	289
#define	TYPE	290
#define	DOCBLOCK	291
#define	STRATOM	292
#define	TAGATOM	293
#define	FLTCONST	294
#define	opINTNAME	295
#define	opINTNAMER	296
#define	opMINUSMINUS	297
#define	opPLUSPLUS	298
#define	STMT_SEP	299
#define	opSHL	300
#define	opSHR	301
#define	opPLASN	302
#define	opMIASN	303
#define	opMLASN	304
#define	opDVASN	305
#define	opMODASN	306
#define	opANDASN	307
#define	opXORASN	308
#define	opORASN	309
#define	opSLASN	310
#define	opSRASN	311
#define	opCAT	312
#define	opEQ	313
#define	opNE	314
#define	opGE	315
#define	opLE	316
#define	opAND	317
#define	opOR	318
#define	opSTREQ	319
#define	opCOLONCOLON	320
#define	opMDASN	321
#define	opNDASN	322
#define	opNTASN	323
#define	opSTRNE	324
#define	UNARY	325


extern YYSTYPE CMDlval;
