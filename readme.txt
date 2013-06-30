Minko SDK

Framework
=========

Sources for the Minko framework.

Examples
========

Example applications created with the Minko fraemwork and its plugins.

Scripts
=======

Set of scripts to ease the deployment of the Minko SDK without using the command line.

premake_vs2010.bat - Windows batch script to create a VS2010 solution and perform other necessary operations such
as copying required DLL into the output folders

premake_clean.bat - Windows batch script to properly clean a VS2010 solution created by premake_vs2010.bat

Templates
=========

This folder contains templates you can use to ease the development of the Minko framework, plugins
or applications.

* vs - Visual Studio templates; can be installed by:
** running the scripts/premake_templates.bat script
** or executing the "premake4 installTemplates" command line
** or by manually copying each template directory into your VS templates directory
(default is $user\Documents\Visual Studio 2012\Templates\ItemTemplates\Projet Visual C++)

Plugins
=======

Sources for Minko's plugins.
