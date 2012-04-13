Restructered Text
=================

Levels 
-------

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


