/** speckle.c *****************************************************************
 *
 * 'Speckle' with the current brush and color on the current drawable.
 *  Size and color can be varied pseudo-randomly.
 *
 *****************************************************************************/

#include <libgimp/gimp.h>

// Define the parameters to be saved for later reuse of plugin

static struct {

  guint numSpeckles, sizeVar, colVar, seed;

} speckleParams  = {

  .numSpeckles = 300,
  .sizeVar = 10,
  .colVar = 128,
  .seed = 1111

};

// Declaration of the mandatory plugin functions

static void query (void);
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
		"plug-in-speckle",									// func. name
		"Speckle!",									// window title
		"Speckle drawable with given brush and color",	// plugin description
		"Uwe Hoffmann",										// author
		"Copyright Uwe Hoffmann",						// copyright notice
		"2019",												// date created
		"_Speckle",								// menu entry name
		"RGB*, GRAY*",										// image types to work with
		GIMP_PLUGIN,										// declare this program as external plugin(NOT in core)
		G_N_ELEMENTS (args), 0,
		args, NULL
	);

	gimp_plugin_menu_register ("plug-in-speckle","<Image>/Uwes-Plugins");

} //static void query(void)...

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
	int i;
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

	drawableWidth	= gimp_drawable_width(drawableID);
	drawableHeight = gimp_drawable_height(drawableID);

	// Begin the main actions context

	gimp_context_push();
	gimp_image_undo_group_start(imageID);

	switch (run_mode) {

		case GIMP_RUN_INTERACTIVE:
			// Get options last values if needed
			gimp_get_data ("plug-in-speckle", &speckleParams);

			// Display the dialog
			//if (! blur_dialog (drawable))
		  	//return;
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

	srand(speckleParams.seed);
	gimp_context_get_foreground(&startRGB);
	brushSize = gimp_context_get_brush_size();
	
	//numSpeckles = 300;

	for (i = 0; i < speckleParams.numSpeckles; i++) {
		speckleX = rand() % drawableWidth;
		speckleY = rand() % drawableHeight;
		speckleCol = rand() % speckleParams.colVar/(speckleParams.colVar*1.0);
		itemRGB = startRGB;
		gimp_rgb_set(&workRGB, speckleCol, speckleCol, speckleCol);
		gimp_rgb_subtract(&itemRGB, &workRGB);
		points[0] = speckleX;
		points[1] = speckleY;
		gimp_context_set_foreground(&itemRGB);
		gimp_context_set_brush_size(brushSize + rand() % speckleParams.sizeVar);
		gimp_paintbrush_default(drawableID, 2, points);
	}

	gimp_image_undo_group_end(imageID);
	
	gimp_context_pop();
	gimp_displays_flush ();
	//	gimp_drawable_detach (drawableID);

	/* End of main actions context */

	//  Finally, set options in the core
	if (run_mode == GIMP_RUN_INTERACTIVE)
		gimp_set_data ("plug-in-speckle", &speckleParams, sizeof (speckleParams));

} // of static void run ...

