/** speckle.c *****************************************************************
 *
 * 'Speckle' with the current brush and color on the current drawable.
 *  Size and color can be varied pseudo-randomly.
 *
 *****************************************************************************/

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

// The parameters and predefinitions (are saved for reuse at the end of plugin!)

static struct {
	guint numSpeckles, sizeVar, colVar, seed;
} speckleParams  = {
	300, 10, 128, 1111 
};

// Declaration of the mandatory plugin functions

static void query (void);
static gboolean showDialog();
static void run (	const gchar *name, gint nparams,	const GimpParam *param,	gint *nreturn_vals, GimpParam **return_vals);

// This structure declaration is also mandatory

GimpPlugInInfo PLUG_IN_INFO = {
	NULL,		//init, optional - called each time gimp starts |
	NULL,		//quit, optional - called before closing gimp   |> very rarely used
	query,	// mandatory pointer to query function
	run		// mandatory pointer to run function
};

/* Needed 'magic' C-Macro */

MAIN()

/*******************************************************************************
 ** query: registration and input arguments definition.
 */

static void query (void) {

	static GimpParamDef args[] = {
		{ GIMP_PDB_INT32, "run-mode", "Run mode"},
		{ GIMP_PDB_IMAGE, "image", "Input image"},
		{ GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
		{ GIMP_PDB_INT32, "numSpeckles", "Number of speckles"},
		{ GIMP_PDB_INT32, "sizeVar", "Variation of max. size in pixels"},
		{ GIMP_PDB_INT32, "colVar", "Variation in brightness (0-255)"},
		{ GIMP_PDB_INT32, "seed", "Random seed"}
	}; // of GimpParamDerf args[] = ...

	gimp_install_procedure (
		"plug-in-speckle",										// func. name
		"Speckle!",													// window title
		"Speckle drawable with given brush and color",	// plugin description
		"Uwe Hoffmann",											// author
		"Copyright Uwe Hoffmann",								// copyright notice
		"2019",														// date created
		"_Speckle",													// menu entry name
		"RGB*, GRAY*",												// image types to work with
		GIMP_PLUGIN,												// declare external plugin (NOT in core)
		G_N_ELEMENTS (args), 0,
		args, NULL
	);

	gimp_plugin_menu_register ("plug-in-speckle","<Image>/Uwes-Plugins");

} // of static void query(void)... ---------------------------------------------

/*******************************************************************************
 ** Create a VERY simple dialog for the bare plugin...
 */

static gboolean showDialog() {

	GtkWidget 	*dialog;
	GtkWidget 	*seedLabel, *seedSpin, *numSpecklesLabel, *numSpecklesSpin,
					*sizeVarLabel, *sizeVarSpin, *colVarLabel, *colVarSpin,
					*contentArea;
	GtkObject 	*seedAdjustment, *numSpecklesAdjustment,
					*sizeVarAdjustment, *colVarAdjustment;
	gboolean 	run;

	// Setup and initialize the whole Gimp GTK+ bunch, without it plugin will crash!
	gimp_ui_init("speckle", FALSE);

	// Create dialog
	dialog = gimp_dialog_new(
		"Speckle", "speckle", NULL, 0, 0, "plug-in-speckle",
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,	GTK_STOCK_OK, GTK_RESPONSE_OK, NULL
	);

	// Get content area of dialog to place widgets
	contentArea = gtk_dialog_get_content_area(GTK_DIALOG (dialog));

	// The numSpeckles widget: fully commented as example ///////////////////////

	// define label
	numSpecklesLabel = gtk_label_new ("Number of speckles:");
	// add label to widget
	gtk_container_add (GTK_CONTAINER (contentArea), numSpecklesLabel);
	// define adjustment for the spin-control (GTK-specific)
	numSpecklesAdjustment = gtk_adjustment_new (speckleParams.numSpeckles, 10, 1111, 1, 10, 10);
	// define widget and add adjustment to it
   numSpecklesSpin = gtk_spin_button_new (GTK_ADJUSTMENT (numSpecklesAdjustment), 1, 0);
	// add a connector to read the value into the speckleParams struct
	g_signal_connect (numSpecklesAdjustment, "value_changed",
   	G_CALLBACK (gimp_int_adjustment_update),
		&speckleParams.numSpeckles
	);
	// now put all this into dialog's content area 
	gtk_container_add (GTK_CONTAINER (contentArea), numSpecklesSpin);
	
	// The sizeVar widget ///////////////////////////////////////////////////////

	sizeVarLabel = gtk_label_new ("Size variation (0-100 pixel):");
	gtk_container_add (GTK_CONTAINER (contentArea), sizeVarLabel);
	sizeVarAdjustment = gtk_adjustment_new (speckleParams.sizeVar, 0, 100, 1, 5, 5);
   sizeVarSpin = gtk_spin_button_new (GTK_ADJUSTMENT (sizeVarAdjustment), 1, 0);
	g_signal_connect (sizeVarAdjustment, "value_changed",
   	G_CALLBACK (gimp_int_adjustment_update),
		&speckleParams.sizeVar
	);
	gtk_container_add (GTK_CONTAINER (contentArea), sizeVarSpin);
	// The colVar widget
	colVarLabel = gtk_label_new ("Intensity variation (0-255):");
	gtk_container_add (GTK_CONTAINER (contentArea), colVarLabel);
	colVarAdjustment = gtk_adjustment_new (3, 1, 16, 1, 5, 5);
   colVarSpin = gtk_spin_button_new (GTK_ADJUSTMENT (colVarAdjustment), 1, 0);
	gtk_container_add (GTK_CONTAINER (contentArea), colVarSpin);
	// The seed widget
	seedLabel = gtk_label_new ("Random seed:");
	gtk_container_add (GTK_CONTAINER (contentArea), seedLabel);
	seedAdjustment = gtk_adjustment_new (3, 1, 16, 1, 5, 5);
   seedSpin = gtk_spin_button_new (GTK_ADJUSTMENT (seedAdjustment), 1, 0);
	gtk_container_add (GTK_CONTAINER (contentArea), seedSpin);
 
	gtk_widget_show_all(dialog);

	// Show and run dialog, WAIT for user action!!!
	gtk_widget_show(dialog);
	run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);
	
	// User action has happend, now destroy widget and free all its resources used
	// and return to make some further action
	gtk_widget_destroy(dialog);
	return run; // return result of the dialog (=TRUE/FALSE)!

} // of showDialog()... --------------------------------------------------------

/*******************************************************************************
 ** run: main actions of the plugin.
 */

static void run (	const gchar *name, gint nparams,	const GimpParam *param,	gint *nreturn_vals, GimpParam **return_vals) {

	static GimpParam  values[1];
	GimpPDBStatusType status = GIMP_PDB_SUCCESS;
	GimpRunMode		run_mode;
	GimpDrawable	*drawable;
	GimpRGB			itemRGB, startRGB, workRGB;
	guint				drawableHeight,  drawableWidth, imageID, drawableID,
						speckleX, speckleY ;
	gfloat speckleCol;
	gdouble points[2];
	gdouble brushSize;

	// Setting mandatory output values
	*nreturn_vals = 1;
	*return_vals  = values;

	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = status;

	run_mode		= param[0].data.d_int32;
	imageID		= param[1].data.d_int32;
	drawableID	= param[2].data.d_int32;

	switch (run_mode) {

		case GIMP_RUN_INTERACTIVE:
			// Get options last values if needed
			gimp_get_data ("plug-in-speckle", &speckleParams);

			// Display the dialog and break if necessary!
			if (! showDialog ())
				return;
			break;

		case GIMP_RUN_NONINTERACTIVE:
			if (nparams != 7)
			  status = GIMP_PDB_CALLING_ERROR; 
			if (status == GIMP_PDB_SUCCESS)
		  		speckleParams.numSpeckles	= param[3].data.d_int32;
				speckleParams.sizeVar		= param[4].data.d_int32;
				speckleParams.colVar			= param[5].data.d_int32;
				speckleParams.seed			= param[6].data.d_int32;
			break;

		case GIMP_RUN_WITH_LAST_VALS:
			//  Get options last values if needed
			gimp_get_data ("plug-in-speckle", &speckleParams);
			break;

		default:	break;
	}	

	drawableWidth	= gimp_drawable_width(drawableID);
	drawableHeight = gimp_drawable_height(drawableID);

	// Begin the main actions context 

	gimp_context_push();
	gimp_image_undo_group_start(imageID);
	srand(speckleParams.seed);
	gimp_context_get_foreground(&startRGB);
	brushSize = gimp_context_get_brush_size();

	// prepare status line to show progress infos
	gimp_progress_init ("Speckle...");

	// Loop over all speckles
	for (int i = 0; i < speckleParams.numSpeckles; i++) {
		
		// calc speckle position
		speckleX = rand() % drawableWidth;
		speckleY = rand() % drawableHeight;
		// calc new random brightness variation from 0-1
		speckleCol = rand() % speckleParams.colVar/(speckleParams.colVar*1.0);
		// subtract the brightness variation from the start color
		// and store result in &itemRGB
		itemRGB = startRGB;
		gimp_rgb_set(&workRGB, speckleCol, speckleCol, speckleCol);
		gimp_rgb_subtract(&itemRGB, &workRGB);
		// make a speckle with active brush in the calculated color
		// and a random size variation
		points[0] = speckleX;
		points[1] = speckleY;
		gimp_context_set_foreground(&itemRGB);
		gimp_context_set_brush_size(brushSize + rand() % speckleParams.sizeVar);
		gimp_paintbrush_default(drawableID, 2, points);
		// show progress in status line
		if (i % (speckleParams.numSpeckles/20) == 0)
			gimp_progress_update ((gdouble) i / (gdouble) (speckleParams.numSpeckles));

	} // calculate next speckle in for (... loop

	gimp_image_undo_group_end(imageID);
	gimp_context_pop();
	gimp_displays_flush ();
	//	gimp_drawable_detach (drawableID);

	//  Finally, save and set options in the core
	if (run_mode == GIMP_RUN_INTERACTIVE)
		gimp_set_data ("plug-in-speckle", &speckleParams, sizeof (speckleParams));
	
	/* End of main actions context */

} // of static void run ... ----------------------------------------------------

