//copyright Antonio Stasio 2020 astasio@gmail.com
#include<gtk/gtk.h>
#include<math.h>

char *title="PixiDraw";
char *version="02/04/2020";

GtkWidget *win;
GtkWidget *canvas;/*Cairo Canvas*/
static cairo_surface_t *surface = NULL;
GtkWidget *vp_new;
GtkToolItem *nuovo;
GtkToolItem *salva;
GtkToolItem *info;
GtkToolItem *chiudi;
GtkWidget *nome_file;/*Il nome del File Immagine*/
GtkWidget *dimensione;/*Risoluzione Immagine*/
GtkWidget *red;
GtkWidget *green;
GtkWidget *blue;

int dim=24;

int resolution(int dimension)
{
    int result;
 switch(dimension){
     case 8 : result=48;break;
     case 16: result=32;break;
     case 24: result=28;break;
     case 32: result=24;break;
     default: break;
 }
         return result;
}

typedef struct{
    int row;
    int col;
char color[6];
}pixel;
pixel img_buffer[1024];

char *convert_color_code(float valore)
{
if(valore>=1.0){return "FF";}
else if(valore>=0.9){return "EE";}
else if(valore>=0.8){return "CC";}
else if(valore>=0.7){return "BB";}
else if(valore>=0.6){return "AA";}
else if(valore>=0.5){return "80";}
else if(valore>=0.4){return "60";}
else if(valore>=0.3){return "50";}
else if(valore>=0.2){return "30";}
else if(valore>=0.1){return "20";}
else if(valore>=0.0){return "00";}
return "";
}

double unconvert_color_code(const char *valore)
{
double risultato;
if((strcmp(valore,"FF"))==0){risultato = 1.0;}
else if((strcmp(valore,"EE"))==0){risultato = 0.9;}
else if((strcmp(valore,"CC"))==0){risultato = 0.8;}
else if((strcmp(valore,"BB"))==0){risultato = 0.7;}
else if((strcmp(valore,"AA"))==0){risultato = 0.6;}
else if((strcmp(valore,"80"))==0){risultato = 0.5;}
else if((strcmp(valore,"60"))==0){risultato = 0.4;}
else if((strcmp(valore,"50"))==0){risultato = 0.3;}
else if((strcmp(valore,"30"))==0){risultato = 0.2;}
else if((strcmp(valore,"20"))==0){risultato = 0.1;}
else if((strcmp(valore,"00"))==0){risultato = 0.0;}
else{risultato=-1.1;}
return risultato;
}

void chiudi_app(GtkWidget *widget,gpointer data)
{ 
	gtk_main_quit();
}

void salva_immagine(GtkWidget *widget,gpointer data)
{ 
    /*GESTIONE COLORI*/
    char color_buffer[dim*dim][8];//Definisco il buffer per i colori
    for(int i=0;i<dim*dim;i+=dim){for(int j=0;j<dim;j++){strcpy(color_buffer[j+i],"       ");}}//alloco l'array 
    int n_colori=1;
    for(int i=0;i<dim*dim;i++){for(int j=0;j<n_colori;j++){
        if((strcmp(img_buffer[i].color,color_buffer[j]))==0){break;}
        else{if(j==n_colori-1){strcpy(color_buffer[n_colori],img_buffer[i].color);n_colori+=1;}}
    }}
  /*FINE GESTIONE COLORI*/
	char file_name[24];
    strcpy(file_name,gtk_entry_get_text(GTK_ENTRY(nome_file)));
    strcat(file_name,".xpm");
	FILE *f=fopen(file_name,"w");
    fprintf(f,"%s","/* XPM */\nstatic char *");
    fprintf(f,"%s",gtk_entry_get_text(GTK_ENTRY(nome_file)));//Nome file
    fprintf(f,"%s","_xpm[] = {\n\"");
    fprintf(f,"%d %d ",dim,dim);//risoluzione immagine
    fprintf(f,"%d",n_colori-1);//Numero colori
    fprintf(f," 1\",\n");//\"  c None\",\n\"a  c #000000\",\n");
    for(int i=1;i<n_colori;i++){fprintf(f,"\"%c    c %s\"\n",i+48,color_buffer[i]);}
    for(int i=0;i<dim*dim;i+=dim){fprintf(f,"%s","\"");
    for(int j=0;j<dim;j++){for(int n=1;n<n_colori;n++){
    if((strcmp(img_buffer[i+j].color,color_buffer[n]))==0){fprintf(f,"%c",n+48);}//Disegno il pixel con il colore giusto
    }}
    fprintf(f,"%s\n","\"");
    } 
    fprintf(f,"%s","\"};");
    fclose(f);
}

void inizialize_img_buffer()
{
    dim=(int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(dimensione));
    for(int i=0;i<dim*dim;i+=dim)
    {
        for(int j=0;j<dim;j++)
        {
         strcpy(img_buffer[j+i].color,"None");
         img_buffer[j+i].row=(j+i)/dim;
         img_buffer[j+i].col=j;
        }
    } 
}

static void clear_surface(void)
{
    cairo_t *cr;
    cr = cairo_create(surface);
 
    cairo_set_source_rgb(cr,0.5,0.5,0.5);
    cairo_rectangle(cr,0,0,dim*resolution(dim),dim*resolution(dim));
    cairo_paint (cr);
    
    cairo_set_source_rgb(cr,0.2,0.2,0.2);
    for(int i=0;i<dim;i++)
    {
        for(int j=0;j<dim;j++)
        {
            cairo_rectangle(cr,j*resolution(dim),i*resolution(dim),resolution(dim),resolution(dim));
        }
    }
    cairo_stroke(cr);
    cairo_destroy(cr);
    
}

static gboolean 
configure_event_cb (GtkWidget           *widget,
                    GdkEventConfigure   *event, 
                        gpointer        data)
{
    
    cairo_surface_t *old_surface=surface;
    cairo_t *cr;
    GdkWindow *window=gtk_widget_get_window(widget);
    surface = gdk_window_create_similar_surface(window,
                                           CAIRO_CONTENT_COLOR,
                                           dim*resolution(dim),dim*resolution(dim));
    clear_surface();    
    if(old_surface){
        //GdkWindow *gdk_window=gtk_widget_get_window(widget);
        //cr = gdk_cairo_create(gdk_window);
        cr = cairo_create(surface);
        cairo_set_source_surface(cr,old_surface, 0, 0);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);
        cairo_destroy(cr);
        cairo_surface_destroy(old_surface);
    } 
    return TRUE;
}

static void paint(GtkWidget *widget,cairo_t *cr, gpointer data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
}

void nuovo_pixidraw(GtkWidget *widget, gpointer data)
{
    inizialize_img_buffer();
    gtk_widget_set_size_request(GTK_WIDGET(canvas),dim*resolution(dim),dim*resolution(dim));
    gtk_widget_set_visible(GTK_WIDGET(canvas),FALSE);   
    gtk_widget_set_visible(GTK_WIDGET(canvas),TRUE);
    clear_surface();
}

void draw_pixel(GtkWidget *widget,GdkEvent* event,gpointer data)
{
    int x, y;
    cairo_t *cr;
    
    float color_red,color_green,color_blue;
    color_red=gtk_range_get_value(GTK_RANGE(red));
    color_green=gtk_range_get_value(GTK_RANGE(green));
    color_blue=gtk_range_get_value(GTK_RANGE(blue));
    
    char color[7];/*COLORE IN FORMATO HTML*/
    strcpy(color,"#");
    strcat(color,convert_color_code(color_red));
    strcat(color,convert_color_code(color_green));
    strcat(color,convert_color_code(color_blue));
    
    gtk_widget_get_pointer(widget,&x,&y);
    GdkWindow *gdk_window=gtk_widget_get_window(widget);
    cr = gdk_cairo_create(gdk_window);
    
    x=(x/resolution(dim))*resolution(dim);
    y=(y/resolution(dim))*resolution(dim);
    if(x > dim*resolution(dim)-resolution(dim) || y > dim*resolution(dim)-resolution(dim))
    {
       return;
    }
    else if(event->button.button==1)/*MOUSE CLICK SINISTRO*/
    {
        cairo_set_source_rgb(cr,color_red,color_green,color_blue);
        cairo_rectangle(cr,x,y,resolution(dim),resolution(dim));
        cairo_fill(cr);
        cairo_destroy(cr);
        int pos_in_array=(x/resolution(dim)+dim*y/resolution(dim));
        strcpy(img_buffer[pos_in_array].color,color);/*MEMORIZZO IL CODICE COLORE NELL'ARRAY*/
        img_buffer[pos_in_array].col=x/resolution(dim);/*MEMORIZZO LA COORDINATA Y NELL'ARRAY*/
        img_buffer[pos_in_array].row=y/resolution(dim);/*MEMORIZZO LA COORDINATA X NELL'ARRAY*/
    }
    else if(event->button.button==3)/*MOUSE CLICK DESTRO*/
    {
        cairo_set_source_rgb(cr,0.5,0.5,0.5);
        cairo_rectangle(cr,x,y,resolution(dim),resolution(dim));
        cairo_fill(cr);
        cairo_set_source_rgb(cr,0.2,0.2,0.2);
        cairo_rectangle(cr,x,y,resolution(dim),resolution(dim));
        cairo_stroke(cr);
        cairo_destroy(cr);
        int pos_in_array=(x/resolution(dim)+dim*y/resolution(dim));
        strcpy(img_buffer[pos_in_array].color,"None");/*MEMORIZZO IL CODICE COLORE NELL'ARRAY*/
        img_buffer[pos_in_array].col=x/resolution(dim);/*MEMORIZZO LA COORDINATA Y NELL'ARRAY*/
        img_buffer[pos_in_array].row=y/resolution(dim);/*MEMORIZZO LA COORDINATA X NELL'ARRAY*/
    }
}

void build_win()
{
	GdkPixbuf *create_pixbuf(const gchar * filename)
    {
        GdkPixbuf *pixbuf;
        GError *error = NULL;
        pixbuf = gdk_pixbuf_new_from_file(filename, &error);
        if(!pixbuf) 
        {
            fprintf(stderr, "%s\n", error->message);
            g_error_free(error);
        }

        return pixbuf;
    }
	
	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	char titolo[29];
	strcpy(titolo,title);
	strcat(titolo," - versione: ");
	strcat(titolo,version);
	gtk_window_set_title(GTK_WINDOW(win),titolo);
    gtk_window_set_icon(GTK_WINDOW(win), create_pixbuf("icon.xpm"));
    gtk_widget_set_size_request(win,600,450);
   	g_signal_connect(win,"destroy",G_CALLBACK(chiudi_app),NULL);
	gtk_widget_show(win);
	
	GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_set_spacing(GTK_BOX(vbox),0);
	gtk_container_add(GTK_CONTAINER(win),vbox);
	gtk_container_set_border_width(GTK_CONTAINER(win),0);
    /* creazione toolbar */
	GtkWidget *toolbar=gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);
	gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(toolbar),0,0,0);
    /*pulsante nuovo disegno*/
	nuovo = gtk_tool_button_new(gtk_image_new_from_icon_name (GTK_STOCK_NEW,48),"Nuovo Disegno");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(nuovo), -1);
    g_signal_connect(nuovo,"clicked",G_CALLBACK(nuovo_pixidraw),NULL);  
    /*pulsante salva*/
    salva = gtk_tool_button_new(gtk_image_new_from_icon_name (GTK_STOCK_SAVE,48),"Salva Disegno");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(salva), -1);
    g_signal_connect(salva,"clicked",G_CALLBACK(salva_immagine),NULL); 
    GtkToolItem *sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep, -1); 
    /*pulsante informazioni*/
	info = gtk_tool_button_new(gtk_image_new_from_icon_name (GTK_STOCK_HELP,48),"Informazioni");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(info), -1);
    //g_signal_connect(info,"clicked",G_CALLBACK(),NULL);
    /*pulsante chiudi applicazione*/
	chiudi = gtk_tool_button_new(gtk_image_new_from_icon_name (GTK_STOCK_QUIT,48),"Chiudi");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(chiudi), -1);
    g_signal_connect(chiudi,"clicked",G_CALLBACK(chiudi_app),NULL);
    /*FINE TOOLBAR */
    /*WIDGETS RISOLUZIONE E NOME FILE*/
    GtkWidget *h1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_container_add(GTK_CONTAINER(vbox),h1);
    
    GtkWidget *lbl_risoluzione=gtk_label_new("Imposta la risoluzione:");
    gtk_container_add(GTK_CONTAINER(h1),lbl_risoluzione);
    GtkAdjustment *adjustment_dim=gtk_adjustment_new(0,16,32,8,0,0);
    dimensione=gtk_spin_button_new(adjustment_dim,0.5,0);
    gtk_container_add(GTK_CONTAINER(h1),dimensione);

    GtkWidget *lbl_nome_file=gtk_label_new("Imposta il nome del file:");
    gtk_container_add(GTK_CONTAINER(h1),lbl_nome_file);
    
    nome_file=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(nome_file),"immagine");
    gtk_entry_set_max_length(GTK_ENTRY(nome_file),20);
    gtk_container_add(GTK_CONTAINER(h1),nome_file);
    gtk_box_set_child_packing(GTK_BOX(h1),nome_file,0,0,0,GTK_PACK_START);
    /*FINE WIDGET NOME FILE*/
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_set_spacing(GTK_BOX(hbox),5);
	gtk_container_add(GTK_CONTAINER(vbox),hbox);
    gtk_box_set_child_packing(GTK_BOX(vbox),hbox,1,1,1,GTK_PACK_START);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),1);
    //BARRA A SINISTRA PER STRUMENTI E IMPOSTAZIONI//
    GtkWidget *vb1=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_set_spacing(GTK_BOX(vb1),5);
	gtk_container_add(GTK_CONTAINER(hbox),vb1);
	gtk_container_set_border_width(GTK_CONTAINER(hbox),2);
    gtk_box_set_child_packing(GTK_BOX(hbox),vb1,0,0,0,GTK_PACK_START);
    //BARRA A PER I COLORI//
    GtkWidget *lbl1 = gtk_label_new("Regolazione Colore");
    gtk_container_add(GTK_CONTAINER(vb1),lbl1);
    GtkWidget *grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(vb1),grid);

    GtkAdjustment *adjustment_red=gtk_adjustment_new(0.0,0.0,1.0,0.01,0,0);
    GtkAdjustment *adjustment_green=gtk_adjustment_new(0.0,0.0,1.0,0.01,0,0);
    GtkAdjustment *adjustment_blue=gtk_adjustment_new(0.0,0.0,1.0,0.01,0,0);
    
    GtkWidget *lbl_red=gtk_label_new("Rosso");
    gtk_grid_attach(GTK_GRID(grid),lbl_red,0,0,1,1);
    red=gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,adjustment_red);
    gtk_widget_set_size_request(GTK_WIDGET(red),100,30);
    gtk_grid_attach(GTK_GRID(grid),red,1,0,1,1);
    
    GtkWidget *lbl_green=gtk_label_new("Verde");
    gtk_grid_attach(GTK_GRID(grid),lbl_green,0,1,1,1);
    green=gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,adjustment_green);
    gtk_widget_set_size_request(GTK_WIDGET(green),100,30);
    gtk_grid_attach(GTK_GRID(grid),green,1,1,1,1);
    
    GtkWidget *lbl_blue=gtk_label_new("Blue");
    gtk_grid_attach(GTK_GRID(grid),lbl_blue,0,2,1,1);
    blue=gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,adjustment_blue);
    gtk_widget_set_size_request(GTK_WIDGET(blue),100,30);
    gtk_grid_attach(GTK_GRID(grid),blue,1,2,1,1);  
    //FINE BARRA A SINISTRA//
	//AREA DISEGNO//
 	GtkWidget *frame=gtk_frame_new("Area Disegno");
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_OUT);
	gtk_box_pack_start(GTK_BOX(hbox),frame,1,1,0);
	gtk_box_set_child_packing(GTK_BOX(hbox),frame,TRUE,TRUE,0,GTK_PACK_START);

	GtkWidget *scroll=gtk_scrolled_window_new(0,0);
	gtk_container_add(GTK_CONTAINER(frame),scroll);
    
    canvas = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(scroll),canvas);
    gtk_box_set_child_packing(GTK_BOX(scroll),canvas,1,1,1,GTK_PACK_START);
    gtk_widget_add_events(canvas,GDK_BUTTON_PRESS_MASK);
    g_signal_connect(canvas,"configure-event",G_CALLBACK(configure_event_cb),NULL);
    g_signal_connect(canvas,"draw",G_CALLBACK(paint),NULL);
    g_signal_connect(canvas,"button-press-event",G_CALLBACK(draw_pixel),NULL);
    gtk_widget_set_events(canvas,gtk_widget_get_events(canvas)|GDK_BUTTON_PRESS_MASK);
    //FINE AREA DISEGNO//
    //STATUSBAR IN FONDO//
	GtkWidget *statusbar=gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox),statusbar,1,1,0);
	gtk_box_set_child_packing(GTK_BOX(vbox),statusbar,FALSE,FALSE,0,GTK_PACK_END);	
    gtk_statusbar_push(GTK_STATUSBAR(statusbar),0,"PixiDraw");
    //FINE STATUSBAR//
	gtk_widget_show_all(GTK_WIDGET(win));	
    gtk_widget_set_visible(GTK_WIDGET(canvas),FALSE);
}

int main(int argc,char **argv)
{
gtk_init(&argc, &argv);
build_win();
gtk_main();
return 0;
}
