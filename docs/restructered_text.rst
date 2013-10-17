Restructered Text
=================

Hierachy
--------

In Restructered text levels are defined by underlining with some character such as = - + * but it is not defined what characters corresponds to what level. To ensure consistensy throughout all documents this is defined here.

=====  ========= ============    ==============  ================
Level  Character Big document    Small document  Notes
-----  --------- ------------    --------------  ----------------
0      ===       Title           Not used        === Both above and below the title
-----  --------- ------------    --------------  ----------------
1      +++       Chapter         Not used        Used for chapter when including a short document where the chapter/title level is already used
-----  --------- ------------    --------------  ----------------
2      ===       Chapter         Title
-----  --------- ------------    --------------  ----------------
3      ---       Section         Section
-----  --------- ------------    --------------  ----------------
4      ...       Subsection      Subsection 
-----  --------- ------------    --------------  ----------------
5      ,,,       Paragraph       Paragraph       Never/seldom used
=====  ========= ============    ==============  ================

Big document
 A large document comprised of multiple files, such as the Kodo documentation

Small document
 Typically a single file such as the a license file, or this file that defines our use of restructered_text


::   

   ==================
   Big document title
   ==================

   Chapter / small document title
   ==============================

   Section
   ----------

   Subsection
   ..........

Math
----

Math support in Sphinx is pretty decent and allows you to escape to a latex environment http://sphinx-doc.org/latest/ext/math.html in the following "a" will be in latex math mode

::

":math:`a`"


We follow the ISO_31 series for notation http://en.wikipedia.org/wiki/ISO_31-11. Note that this has been superseeded by the "ISO/IEC 80000" standard familiy - which unfortunately is not freely available.

This means that that contants are lowercase, variables are lowercase italic, vectors are lowercase bold, and matrices are uppercase bold

::

   This is an easy equation :math:`a \cdot b`
   
   A variable is written as :math:`\mathrm{\pi}`
   
   A constant is written as :math:`a`
   
   A vector is written as :math:`\boldsymbol{a}`
   
   A matrix is written as :math:`\boldsymbol{A}`


This is an easy equation :math:`a \cdot b`

A variable is written as :math:`\mathrm{\pi}`

A constant is written as :math:`a`

A vector is written as :math:`\boldsymbol{a}`

A matrix is written as :math:`\boldsymbol{A}`


**Note that Github currently have not managed to support redering of math, which is why the above examples does not display correctly here

