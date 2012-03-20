@q 1999 by Max Boehm @>

\hoffset=-2.6truemm

\input epsf

\font\abacusfont=cmssq8 scaled \magstep1
\font\seriesfont=cmr10 scaled \magstep2

\def\ABACUS{{\abacusfont ABACUS}}

\def\BeginArg{\smallskip\noindent Arguments:}
\def\EndArg{\smallskip\noindent}  

\def\BeginReturn{\smallskip\noindent Return Value:
  \par\hangindent\parindent
}
\def\EndReturn{
  \smallskip\noindent
}  

% macro to document the arguments of a function

\newdimen\oldparindent

\def\docarg#1#2{
  \par
  #1
  \oldparindent=\parindent
  \par\nobreak
  \advance\parindent by \oldparindent
  \hangindent\parindent
  #2
  \parindent=\oldparindent
}


\def\title{Parallel ABACUS}
\def\topofcontents{\null\vfill
  \centerline{\seriesfont ANGEWANDTE MATHEMATIK UND INFORMATIK}
  \vskip 8pt
  \centerline{\seriesfont UNIVERSIT\"AT ZU K\"OLN}
  \vskip 8pt
  \centerline{Report 99.359}
  \vskip 60pt
  \centerline{\titlefont Parallel ABACUS -- Implementation}
  \vskip 15pt
  \centerline{June 1999}
  \vskip 15pt
  \centerline{Max B\"ohm}
  \vfill
}

\def\botofcontents{\vfill
\noindent
Copyright \copyright\ Universit\"at zu K\"oln, Universit\"at Heidelberg
\bigskip\noindent
This work has been partially supported by DFG-Projekt Ju~204/4-2, Re~776/5-3.
}

@s bool int
@s ABA_MASTER int
@s ABA_SUB int
@s ABA_CONSTRAINT int
@s ABA_VARIABLE int
@s ABA_ARRAY int
@s ABA_BUFFER int
@s ABA_CONVAR int
@s ABA_FASTSET int
@s ABA_OPTSENSE int
@s ABA_CSENSE int 
@s ABA_ABACUSROOT int 
@s ABA_STRING int 
@s ABA_POOL int 

@s ABA_ACTIVE int
@s ABA_ARRAY int
@s ABA_BALANCER int
@s ABA_BOUNDBRANCHRULE int
@s ABA_BRANCHRULE int
@s ABA_BUFFER int
@s ABA_COLVAR int
@s ABA_CONBRANCHRULE int
@s ABA_CONCLASS int
@s ABA_CONSTRAINT int
@s ABA_CONVAR int
@s ABA_FSVARSTAT int
@s ABA_ID int
@s ABA_IDMAP int
@s ABA_LP int
@s ABA_LPVARSTAT int
@s ABA_MASTER int
@s ABA_MESSAGE int
@s ABA_NOTIFICATION int
@s ABA_NOTIFYSERVER int
@s ABA_NUMCON int
@s ABA_NUMVAR int
@s ABA_OPENSUB int
@s ABA_OSTREAM int
@s ABA_PARMASTER int
@s ABA_POOL int
@s ABA_POOLSLOT int
@s ABA_POOLSLOTREF int
@s ABA_ROWCON int
@s ABA_SETBRANCHRULE int
@s ABA_SLACKSTAT int
@s ABA_SPARVEC int
@s ABA_SROWCON int
@s ABA_STANDARDPOOL int
@s ABA_SUB int
@s ABA_VALBRANCHRULE int
@s ABA_VARIABLE int
@s ABA_VARTYPE int

@s ABA_COWTIMER int 
@s ABA_DUALBOUND int 
@s ABA_MESSAGE int 
@s ABA_MESSAGEBASE int 
@s ABA_BROADCAST int 
@s ABA_NOTIFICATION int 
@s ABA_ID int 
@s ABA_IDMAP int 
@s ABA_MTSERVER int 

@s ACE_SOCK_Stream int 
@s ACE_Thread_Mutex int 
@s ACE_Thread_Manager int 
@s ACE_Acceptor int 
@s ACE_Connector int 
@s ACE_INET_Addr int 
@s ACE_Condition int 
@s ACE_Error int 
@s ACE_OS int 
@s ACE_Task int 

@s EDGE int
@s DEGREE int
@s SUBTOUR int
@s TSPSUB int
@s TSPMASTER int
@s STATUS int



@i parmaster.wh
@i parmaster.w


@i mtserver.wh
@i mtserver.w

@i notifyserver.wh
@i notifyserver.w

@i subserver.wh
@i subserver.w

@i balancer.wh
@i balancer.w


@i messagebase.wh
@i messagebase.w

@i message.wh
@i message.w

@i broadcast.wh
@i broadcast.w

@i notification.wh
@i notification.w

@** HELPER CLASSES.

@i id.wh
@i id.w

@i idmap.wh
@i idmap.winc

@i intset.wh
@i intset.w

@i dualbound.wh
@i dualbound.w

@** SERIALIZATION.

This section contains extensions for serialization
of several \ABACUS\ classes.

@i Tex/serialization.w


@* REFERENCES.

\def\reference#1{\par\hangindent46pt\noindent\ignorespaces
  \hbox to 46pt{[#1]\hfil}\ignorespaces}

 
\reference{ACE} D. C. Schmidt,
          The {ADAPTIVE} {C}ommunication {E}nvironment {(ACE)},
         {\sl Washington University\/},
         http://www.cs.wustl.edu/\~{}schmidt/ACE.html.

\reference{Ref22}ABACUS 2.2, User's Guide and Reference Manual,
                  HTML version,
                 Universit\"at zu K\"oln, Universit\"at Heidelberg, 1998,
http://www.informatik.uni-koeln.de/ls\_juenger/projects/abacus/html/ manual.html.

\reference{Dist22}ABACUS 2.2, Software Distribution,
                 Universit\"at zu K\"oln, Universit\"at Heidelberg, 1998,
http:// www.informatik.uni-koeln.de/ls\_juenger/projects/abacus/distribution.html.


@* INDEX AND SECTION NAMES.

