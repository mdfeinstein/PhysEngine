#pragma once
#include <wx/wx.h>
#include "app.h"

class MyApp : public wxApp {
    public:
        Simulator& sim;
        MyApp(Simulator& sim) : sim(sim) {}
        virtual bool OnInit() {
            MyFrame* frame = new MyFrame(sim);
            frame->Show(true);
            return true;
        }
    };


    // Entry point
int main(int argc, char** argv) {
    Simulator sim(0.1);  // Create a Simulator instance
    MyApp* app = new MyApp(sim);  // Instantiate MyApp with the Simulator
    wxApp::SetInstance(app);
    return wxEntry(argc, argv);
}