<!--
==============================================================
 File: neddoc.xsl - part of OMNeT++
==============================================================

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (C) 2002-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="html" indent="yes" encoding="iso-8859-1"/>


<!-- directory to generate output -->
<xsl:param name="outputdir" select="'html'"/>

<!-- to enable images (screenshots), set this to the name of the images.xml file generated by gned -c option -->
<xsl:param name="imagesxml" select="''"/>


<xsl:param name="inputdoc" select="/"/>

<xsl:key name="module" match="//simple-module|//compound-module" use="@name"/>
<xsl:key name="msg-or-class" match="//message|//class|//struct" use="@name"/>
<xsl:key name="channel" match="//channel" use="@name"/>

<xsl:key name="image" match="//image" use="concat(@name,':',@nedfilename)"/>

<!-- ROOT -->
<xsl:template match="/">
   <xsl:document href="{$outputdir}/index.html">
      <html>
         <head>
            <title>Model documentation -- generated from NED files</title>
         </head>
         <frameset cols="35%,65%">
            <frameset rows="60%,40%">
               <frame src="modules.html" name="componentsframe"/>
               <frame src="files.html" name="filesframe"/>
            </frameset>
            <frame src="overview.html" name="mainframe"/>
         </frameset>
         <noframes>
            <h2>Frame Alert</h2>
            <p>This document is designed to be viewed using HTML frames. If you see this message,
            you are using a non-frame-capable browser.</p>
         </noframes>
      </html>
   </xsl:document>

   <xsl:document href="{$outputdir}/style.css" method="text">
      body,td,p,ul,ol,li,h1,h2,h3,h4 {font-family:arial,sans-serif }
      body,td,p,ul,ol,li { font-size:10pt }
      h1 { font-size:18pt; text-align:center }
      pre.comment { font-size:10pt; padding-left:5pt }
      pre.src { font-size:8pt; background:#E0E0E0; padding-left:5pt }
      th { font-size:10pt; text-align:left; vertical-align:top; background:#E0E0f0 }
      td { font-size:10pt; text-align:left; vertical-align:top }
      tt { font-family:Courier,Courier New,Fixed,Terminal }
      .navbar     { font-size:8pt; }
      .navbarlink { font-size:8pt; }
      .indextitle { font-size:12pt; }
      .comptitle  { font-size:14pt; }
      .subtitle   { font-size:12pt; margin-bottom: 3px}
      .footer     { font-size:8pt; margin-top:0px; text-align:center; color:#303030; }
      FIXME.paramtable { border:2px ridge; border-collapse:collapse;}
      img.screenshot { border:none }
      .src-keyword { font-weight:bold }
      .src-comment { font-style:italic; color:#404040 }
      .src-string  { color:#006000 }
      .src-number  { color:#0000c0 }
   </xsl:document>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'simplemodules.html'"/>
      <xsl:with-param name="content">
        <xsl:call-template name="create-simplemodule-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'compoundmodules.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-compoundmodule-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'modules.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-module-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'channels.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-channel-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'networks.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-network-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'messages.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-message-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'classes.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-class-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'structs.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-struct-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'enums.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-enum-index"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'files.html'"/>
      <xsl:with-param name="content">
         <xsl:call-template name="create-fileindex"/>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'overview.html'"/>
      <xsl:with-param name="content">
         <center><h1>OMNeT++ Model Documentation</h1></center>
         <center><i>Generated from NED files</i></center>
         <p>This documentation has been generated from NED files. Use the links in
         the left frames to navigate around.</p>
         <p>Generated by opp_neddoc.</p>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'unknown-module.html'"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Unknown Module Type</h2>
         <p>The module you clicked on is not defined in the NED files documented 
         in these pages. (It may still be defined in other NED files, external
         to this documentation.)</p>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:call-template name="write-html-page">
      <xsl:with-param name="href" select="'ambiguous-module.html'"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Ambiguous Module Type</h2>
         <p>The module you clicked on has multiple definitions in the documented NED files.
         Please choose the correct definition from the module index.</p>
      </xsl:with-param>
   </xsl:call-template>

   <xsl:document href="{$outputdir}/tags.xml" indent="yes">
      <xsl:call-template name="create-tags"/>
   </xsl:document>

   <xsl:apply-templates/>

</xsl:template>


<!-- INDEX PAGES -->

<xsl:template name="create-simplemodule-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'simple'"/>
   </xsl:call-template>
   <h3 class="indextitle">Simple Modules</h3>
   <ul>
      <xsl:for-each select="//simple-module">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
            <!--
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
            -->
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-compoundmodule-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'compound'"/>
   </xsl:call-template>
   <h3 class="indextitle">Compound Modules</h3>
   <ul>
      <xsl:for-each select="//compound-module">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
            <!--
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
            -->
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-module-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'all'"/>
   </xsl:call-template>
   <h3 class="indextitle">All Modules</h3>
   <ul>
      <xsl:for-each select="//simple-module|//compound-module">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
            <!--
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
            -->
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-channel-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'channels'"/>
   </xsl:call-template>
   <h3 class="indextitle">Channels</h3>
   <ul>
      <xsl:for-each select="//channel">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
            <!--
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
            -->
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-network-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'networks'"/>
   </xsl:call-template>
   <h3 class="indextitle">Networks</h3>
   <ul>
      <xsl:for-each select="//network">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
            <!--
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
            -->
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-message-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'messages'"/>
   </xsl:call-template>
   <h3 class="indextitle">Messages</h3>
   <ul>
      <xsl:for-each select="//message">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-class-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'classes'"/>
   </xsl:call-template>
   <h3 class="indextitle">Classes</h3>
   <ul>
      <xsl:for-each select="//class">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-struct-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'structs'"/>
   </xsl:call-template>
   <h3 class="indextitle">Structs</h3>
   <ul>
      <xsl:for-each select="//struct">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-enum-index">
   <xsl:call-template name="print-navbar">
      <xsl:with-param name="where" select="'enums'"/>
   </xsl:call-template>
   <h3 class="indextitle">Enums</h3>
   <ul>
      <xsl:for-each select="//enum">
         <xsl:sort select="@name"/>
         <li>
            <a href="{concat(@name,'-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@name"/></a>
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-fileindex">
   <h3 class="indextitle">NED and MSG Files</h3>
   <ul>
      <xsl:for-each select="//ned-file">
         <xsl:sort select="@filename"/>
         <li>
            <a href="{concat('file-',generate-id(.))}.html" target="mainframe"><xsl:value-of select="@filename"/></a>
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-tags">
   <neddoc-tags>
      <xsl:for-each select="//simple-module|//compound-module|//channel|//network">
         <xsl:sort select="concat(local-name(.),':',@name)"/>
         <tag type="{local-name(.)}"
              name="{@name}"
              htmlfile="{concat(@name,'-',generate-id(.))}.html"
              nedfile="{ancestor::ned-file/@filename}"
              comment="{@banner-comment}" />
      </xsl:for-each>
   </neddoc-tags>
</xsl:template>


<!-- COMPONENT PAGES -->

<xsl:template match="channel">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Channel <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-attrs"/>
         <xsl:call-template name="print-channel-used-in"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="simple-module">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Simple Module <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-params"/>
         <xsl:call-template name="print-gates"/>
         <xsl:call-template name="print-module-used-in"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="compound-module">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Compound Module <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-screenshot"/>
         <xsl:call-template name="print-params"/>
         <xsl:call-template name="print-gates"/>
         <xsl:call-template name="print-uses"/>
         <xsl:call-template name="print-module-used-in"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="network">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Network <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-type"/>
         <xsl:call-template name="print-substparams"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="message">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Message <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-extends"/>
         <xsl:call-template name="print-properties"/>
         <xsl:call-template name="print-fields"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="class">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Class <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-extends"/>
         <xsl:call-template name="print-properties"/>
         <xsl:call-template name="print-fields"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="struct">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Struct <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-extends"/>
         <xsl:call-template name="print-properties"/>
         <xsl:call-template name="print-fields"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="enum">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat(@name,'-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">Enum <i><xsl:value-of select="@name"/></i></h2>
         <xsl:call-template name="print-file"/>
         <xsl:call-template name="process-comment"/>
         <xsl:call-template name="print-extends"/>
         <xsl:call-template name="print-properties"/>
         <xsl:call-template name="print-enumfields"/>
         <xsl:call-template name="print-source"/>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template>

<xsl:template match="ned-file">
   <xsl:call-template name="write-component-page">
      <xsl:with-param name="href" select="concat('file-',generate-id(.),'.html')"/>
      <xsl:with-param name="content">
         <h2 class="comptitle">File <i><xsl:value-of select="@filename"/></i></h2>
         <xsl:call-template name="process-comment"/>
         <h3 class="subtitle">Contains:</h3>
         <ul>
            <xsl:for-each select="simple-module|compound-module|channel|network|message|class|struct|enum">
               <xsl:sort select="@name"/>
               <!--
               <xsl:call-template name="print-componentref"/>
               -->
               <li>
                  <a href="{concat(@name,'-',generate-id(.))}.html"><xsl:value-of select="@name"/></a>
                  <i> (<xsl:value-of select="local-name()"/>)</i>
               </li>
            </xsl:for-each>
         </ul>
      </xsl:with-param>
   </xsl:call-template>
   <xsl:apply-templates/>
</xsl:template>


<!-- HELPER TEMPLATES -->

<xsl:template name="write-html-page">
   <xsl:param name="href"/>
   <xsl:param name="content"/>
   <xsl:document href="{$outputdir}/{$href}" method="html" indent="yes">
      <html>
         <head>
            <link rel="stylesheet" type="text/css" href="style.css" />
         </head>
         <body>
            <xsl:copy-of select="$content"/>
         </body>
      </html>
   </xsl:document>
</xsl:template>

<xsl:template name="write-component-page">
   <xsl:param name="href"/>
   <xsl:param name="content"/>
   <xsl:document href="{$outputdir}/{$href}" method="html" indent="yes">
      <html>
         <head>
            <link rel="stylesheet" type="text/css" href="style.css" />
         </head>
         <body>
            <!--TBD styling: <p style="color=red;font-weight=bold;">OMNeT++ model documentation</p> -->
            <xsl:copy-of select="$content"/>
            <!--TBD styling: <hr width="100%"/><p class="footer">Generated by opp_neddoc</p> -->
         </body>
      </html>
   </xsl:document>
</xsl:template>

<xsl:template name="print-navbar">
   <xsl:param name="where"/>
   <p class="navbar">
      <xsl:text>modules (</xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'simple'"/>
          <xsl:with-param name="link" select="'simplemodules.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text>, </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'compound'"/>
          <xsl:with-param name="link" select="'compoundmodules.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text>, </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'all'"/>
          <xsl:with-param name="link" select="'modules.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text>) - </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'channels'"/>
          <xsl:with-param name="link" select="'channels.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text> - </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'networks'"/>
          <xsl:with-param name="link" select="'networks.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text> - </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'messages'"/>
          <xsl:with-param name="link" select="'messages.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text> - </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'classes'"/>
          <xsl:with-param name="link" select="'classes.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text> - </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'structs'"/>
          <xsl:with-param name="link" select="'structs.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
      <xsl:text> - </xsl:text>
      <xsl:call-template name="print-navbarlink">
          <xsl:with-param name="label" select="'enums'"/>
          <xsl:with-param name="link" select="'enums.html'"/>
          <xsl:with-param name="where" select="$where"/>
      </xsl:call-template>
   </p>
</xsl:template>

<xsl:template name="print-navbarlink">
   <xsl:param name="label"/>
   <xsl:param name="link"/>
   <xsl:param name="where"/>
   <xsl:choose>
      <xsl:when test="$label=$where">
         <span class="navbarlink"><xsl:value-of select="$label"/></span>
      </xsl:when>
      <xsl:otherwise>
         <a href="{$link}" class="navbarlink"><xsl:value-of select="$label"/></a>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template name="print-file">
   <xsl:if test="ancestor::ned-file/@filename">
      <p><b>File: <a href="{concat('file-',generate-id(ancestor::ned-file))}.html"><xsl:value-of select="ancestor::ned-file/@filename"/></a></b></p>
   </xsl:if>
</xsl:template>

<xsl:template name="print-params">
   <xsl:if test="params/param">
      <h3 class="subtitle">Parameters:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Type</th>
            <th>Description</th>
         </tr>
         <xsl:for-each select="params/param">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="100">
                  <i>
                  <xsl:choose>
                     <xsl:when test="@data-type"><xsl:value-of select="@data-type"/></xsl:when>
                     <xsl:otherwise>numeric</xsl:otherwise>
                  </xsl:choose>
                  </i>
               </td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-gates">
   <xsl:if test="gates/gate">
      <h3 class="subtitle">Gates:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Direction</th>
            <th>Description</th>
         </tr>
         <xsl:for-each select="gates/gate">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/><xsl:if test="@is-vector='true'"> [ ]</xsl:if></td>
               <td width="100"><i><xsl:value-of select="@direction"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-attrs">
   <xsl:if test="channel-attr">
      <h3 class="subtitle">Attributes:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Value</th>
            <th>Description</th>
         </tr>
         <xsl:for-each select="channel-attr">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="300"><i><xsl:value-of select="@value"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-substparams">
   <xsl:if test="substparams/substparam">
      <h3 class="subtitle">Parameter values:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Value</th>
            <th>Comment</th>
         </tr>
         <xsl:for-each select="substparams/substparam">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="300"><i><xsl:value-of select="@value"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-fields">
   <xsl:if test="fields/field">
      <h3 class="subtitle">Fields:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Type</th>
            <th>Description</th>
         </tr>
         <xsl:for-each select="fields/field">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="100">
                  <i>
                  <xsl:choose>
                     <!-- FIXME: should put hyperlink on known compound types -->
                     <xsl:when test="@is-vector='true'"><xsl:value-of select="concat(@data-type,'[',@vector-size,']')"/></xsl:when>
                     <xsl:otherwise><xsl:value-of select="@data-type"/></xsl:otherwise>
                  </xsl:choose>
                  </i>
               </td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-properties">
   <xsl:if test="properties/property">
      <h3 class="subtitle">Properties:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Value</th>
            <th>Description</th>
         </tr>
         <xsl:for-each select="properties/property">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="100"><i><xsl:value-of select="@value"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-enumfields">
   <xsl:if test="enum-fields/enum-field">
      <h3 class="subtitle">Enum values:</h3>
      <table class="paramtable">
         <tr>
            <th>Name</th>
            <th>Value</th>
            <th>Description</th>
         </tr>
         <xsl:for-each select="enum-fields/enum-field">
            <!-- <xsl:sort select="@name"/> -->
            <tr>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="100"><i><xsl:value-of select="@value"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-uses">
   <xsl:if test="key('module',.//submodule/@type-name)">
      <h3 class="subtitle">Contains the following modules:</h3>
      <table>
        <xsl:for-each select="key('module',.//submodule/@type-name)">
           <xsl:sort select="@name"/>
           <xsl:call-template name="print-componentref"/>
        </xsl:for-each>
        <xsl:for-each select=".//submodule/@type-name[not(key('module',.))]">
           <xsl:call-template name="print-unresolved-componentref"/>
        </xsl:for-each>
      </table>
   </xsl:if>
   <xsl:if test="key('channel',.//conn-attr[@name='channel']/@value)">
      <h3 class="subtitle">Contains the following channels:</h3>
      <table>
         <xsl:for-each select="key('channel',.//conn-attr[@name='channel']/@value)">
            <xsl:sort select="@name"/>
            <xsl:call-template name="print-componentref"/>
         </xsl:for-each>
        <xsl:for-each select=".//conn-attr[@name='channel']/@value[not(key('channel',.))]">
           <xsl:call-template name="print-unresolved-componentref"/>
        </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-module-used-in">
   <xsl:variable name="name" select="@name"/>
   <xsl:if test="//compound-module[.//submodule[@type-name=$name]]">
      <h3 class="subtitle">Used in compound modules:</h3>
      <table>
         <xsl:for-each select="//compound-module[.//submodule[@type-name=$name]]">
            <xsl:call-template name="print-componentref"/>
         </xsl:for-each>
      </table>
   </xsl:if>
   <xsl:if test="//network[@type-name=$name]">
      <h3 class="subtitle">Networks:</h3>
      <table>
         <xsl:for-each select="//network[@type-name=$name]">
            <xsl:call-template name="print-componentref"/>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-channel-used-in">
   <xsl:variable name="name" select="@name"/>
   <xsl:if test="//compound-module[.//conn-attr[@name='channel' and @value=$name]]">
      <h3 class="subtitle">Used in compound modules:</h3>
      <table>
         <xsl:for-each select="//compound-module[.//conn-attr[@name='channel' and @value=$name]]">
            <xsl:call-template name="print-componentref"/>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-type">
   <xsl:if test="key('module',@type-name)">
      <h3 class="subtitle">Instantiates the following module:</h3>
      <table>
         <xsl:for-each select="key('module',@type-name)">
            <xsl:sort select="@name"/>
            <xsl:call-template name="print-componentref"/>
         </xsl:for-each>
        <xsl:for-each select="@module-type[not(key('module',.))]">
           <xsl:call-template name="print-unresolved-componentref"/>
        </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-extends">
   <xsl:if test="@extends-name">
      <h3 class="subtitle">Extends:</h3>
      <table>
         <xsl:for-each select="key('msg-or-class',@extends-name)">
            <xsl:sort select="@name"/>
            <xsl:call-template name="print-componentref"/>
         </xsl:for-each>
        <xsl:for-each select="@extends-name[not(key('msg-or-class',.))]">
           <xsl:call-template name="print-unresolved-componentref"/>
        </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-componentref">
   <tr>
      <td>
         <a href="{concat(@name,'-',generate-id(.))}.html"><xsl:value-of select="@name"/></a>
      </td>
      <td>
         <xsl:choose>
            <xsl:when test="@banner-comment">
               <span class="briefcomment"><xsl:value-of select="@banner-comment"/></span>
            </xsl:when>
            <xsl:otherwise><i>(no description)</i></xsl:otherwise>
         </xsl:choose>
      </td>
   </tr>
</xsl:template>

<xsl:template name="print-unresolved-componentref">
   <tr>
      <td>
         <xsl:value-of select="."/>
      </td>
      <td>
         <i>(unknown -- not in documented files)</i>
      </td>
   </tr>
</xsl:template>

<xsl:template name="print-screenshot">
   <xsl:if test="$imagesxml!=''">
      <xsl:variable name="context" select="."/>
      <xsl:variable name="modname" select="@name"/>
      <xsl:variable name="nedfilename" select="ancestor::ned-file/@filename"/>
      <xsl:for-each select="document($imagesxml)">
         <xsl:for-each select="key('image',concat($modname,':',$nedfilename))">
            <img class="screenshot" src="{@imgfilename}" ismap="yes" usemap="#screenshot" width="{@width}" height="{@height}"/>
            <map name="screenshot">
               <xsl:for-each select="area">
                  <xsl:variable name="coords" select="@coords"/>
                  <xsl:variable name="submodname" select="@name"/>
                  <xsl:variable name="submodtype1" select="$context/submodules/submodule[@name=$submodname]/@type-name"/>
                  <xsl:variable name="submodtype2" select="$context/submodules/submodule[@name=$submodname]/@like-name"/>
                  <xsl:variable name="submodtype">
                     <xsl:choose>
                        <xsl:when test="$submodtype2"><xsl:value-of select="$submodtype2"/></xsl:when>
                        <xsl:otherwise><xsl:value-of select="$submodtype1"/></xsl:otherwise>
                     </xsl:choose>
                  </xsl:variable>
                  <xsl:for-each select="$inputdoc">
                     <xsl:variable name="submodtypenode" select="key('module',$submodtype)"/>
                     <xsl:variable name="url">
                        <xsl:call-template name="resolve-moduleurl">
                           <xsl:with-param name="module" select="$submodtypenode"/>
                        </xsl:call-template> 
                     </xsl:variable>
                     <area shape="rect" coords="{$coords}" href="{$url}" alt="{$submodname}: {$submodtype}"/>
                  </xsl:for-each>
               </xsl:for-each>
            </map>
          </xsl:for-each>
      </xsl:for-each>
   </xsl:if>
</xsl:template>

<xsl:template name="resolve-moduleurl">
   <xsl:param name="module"/>
   <xsl:choose>
      <xsl:when test="count($module)>1">
         ambiguous-module.html
      </xsl:when>
      <xsl:when test="$module">
         <xsl:value-of select="concat($module/@name,'-',generate-id($module),'.html')"/>
      </xsl:when>
      <xsl:otherwise>
         unknown-module.html
      </xsl:otherwise>
   </xsl:choose>   
</xsl:template>

<xsl:template name="print-source">
   <xsl:if test="@source-code">
      <h3 class="subtitle">Source code:</h3>
      <pre class="src"><xsl:value-of select="@source-code"/></pre>
   </xsl:if>
</xsl:template>


<xsl:template name="process-tablecomment">
   <xsl:param name="comment" select="@banner-comment|@right-comment"/>
   <xsl:choose>
      <xsl:when test="$comment">
         <span class="comment"><xsl:value-of select="$comment"/></span>
      </xsl:when>
      <xsl:otherwise> </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template name="process-comment">
   <xsl:param name="comment" select="@banner-comment"/>
   <xsl:choose>
      <xsl:when test="$comment">
         <pre class="comment"><xsl:value-of select="$comment"/></pre>
      </xsl:when>
      <xsl:otherwise><p>(no description)</p></xsl:otherwise>
   </xsl:choose>
</xsl:template>

</xsl:stylesheet>


