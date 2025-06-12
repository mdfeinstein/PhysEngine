#pragma once
#include <wx/wx.h>
#include "app.h"
#include <thread>
#include "Simulator.h"

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

// Run the wxWidgets application in a separate thread
void run_wx_app(Simulator& sim) {
    MyApp* app = new MyApp(sim);
    wxApp::SetInstance(app);
    int argc = 0;
    char** argv = nullptr;
    wxEntryStart(argc, argv);
    app->OnInit();
    app->OnRun();
    app->OnExit();
    wxEntryCleanup();
}

// Run the wxWidgets application in a separate thread
void run_wx_app_newthread(Simulator& sim) {
    MyApp* app = new MyApp(sim);
    wxApp::SetInstance(app);
    int argc = 0;
    char** argv = nullptr;
    std::thread t([&]() {
        wxEntryStart(argc, argv);
        app->OnInit();
        app->OnRun();
        app->OnExit();
        wxEntryCleanup();
    });
    t.detach();
}