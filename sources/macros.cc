% macro to document the arguments of a function

\newdimen\oldparindent

\def\bab{branch-and-bound}
\def\bac{branch-and-cut}  
\def\lpbab{linear programming based \bab}
\def\pbac{parallel branch-and-cut}
\def\ref#1{[#1]}

\def\item#1{\par\hangindent18pt\noindent\ignorespaces
  \hbox to 18pt{#1\hfil}\ignorespaces}
\def\shortitem#1{\par\hangindent8pt\noindent\ignorespaces
  \hbox to 8pt{#1\hfil}\ignorespaces}
\def\longitem#1{\par\hangindent27pt\noindent\ignorespaces
  \hbox to 27pt{#1\hfil}\ignorespaces}

\font\abacusfont=cmssq8 scaled \magstep1
\def\ABACUS{{\abacusfont ABACUS}}

\voffset-0.8cm
\hoffset-0.35cm
\pagewidth=16.5cm
\pageheight=24.6cm
\fullpageheight=25.6cm
\pageshift=0pt
\setpage

