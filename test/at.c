#include <stdio.h>
#include <stdlib.h>
#include <libbonobo.h>
#include "Registry.h"
#include "Event.h"
#include "accessible.h"
#include "listener.h"
#include "desktop.h"

int
main(int argc, char **argv)
{
        CORBA_Environment ev;
        CORBA_Object oclient;
        char *obj_id;
        CORBA_long i;
        CORBA_short n_desktops;
        CORBA_long j;
        CORBA_long n_apps;
        Accessibility_Desktop desktop;
        Accessibility_Application app;

        Accessibility_Registry registry;
        Listener *listener;

        CORBA_exception_init(&ev);

        if (!bonobo_init (&argc, argv))
          {
            g_error ("Could not initialize Bonobo");
          }

        obj_id = "OAFIID:Accessibility_Registry:proto0.1";

        oclient = oaf_activate_from_id (obj_id, 0, NULL, &ev);
        if (ev._major != CORBA_NO_EXCEPTION) {
                fprintf(stderr,
                ("Accessibility app error: exception during registry activation from id: %s\n"),
                        CORBA_exception_id(&ev));
                CORBA_exception_free(&ev);
                exit(-1);
        }

        if (CORBA_Object_is_nil (oclient, &ev))
          {
            g_error ("Could not locate registry");
          }

        bonobo_activate ();

        listener = listener_new ();

        registry = (Accessibility_Registry) oclient;

        Accessibility_Registry_registerGlobalEventListener
                                   (registry,
                                    (Accessibility_EventListener)
                                         bonobo_object_corba_objref (bonobo_object (listener)),
                                    "test",
                                    &ev);

        fprintf (stderr, "AT callback registered.\n");


            n_desktops = Accessibility_Registry_getDesktopCount (registry, &ev);

            for (i=0; i<n_desktops; ++i)
              {
                desktop = Accessibility_Registry_getDesktop (registry, i, &ev);
                fprintf (stderr, "desktop %d name: %s\n", i,
                         Accessibility_Desktop__get_name (desktop, &ev));
                n_apps = Accessibility_Desktop__get_childCount (desktop, &ev);
                fprintf (stderr, "desktop has %d apps:\n", n_apps);
                for (j=0; j<n_apps; ++j)
                  {
                    app = (Accessibility_Application) Accessibility_Desktop_getChildAtIndex (desktop, j, &ev);
                    fprintf (stderr, "app %d name: %s\n", j,
                             Accessibility_Application__get_name (app, &ev));
                  }
              }

            fprintf (stderr, "finished querying desktop(s).\n");
            bonobo_main ();
               /* needed by at because it is a server ? */
        exit(0);
}
