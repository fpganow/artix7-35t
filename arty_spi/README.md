## To recreate this project from Vivado 2017.4, do the following:

# Launch Vivado

# Open the TCL console

# Enter the directory where the project.tcl file is located, in my case it is:
cd /work/git/artix7-35t/arty_spi

# Import the project with the source command
source ./project.tcl

# A new project will have been created in the ./arty_spi directory

# I believe there must be a better way to export/import a project, one day.... I will update the repo, but what you have in front of you should work

# Just make a simple Board Support Package with no optional packages

# Import the project under the export/arty_lwip
- Change referenced BSP by right-clicking on project name and selecting properties
- Then uncheck the existing reference to an non existing project
- check the new bsp that you created


