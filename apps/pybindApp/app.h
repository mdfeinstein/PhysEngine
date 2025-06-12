#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/dcbuffer.h>
#include "Simulator.h"
#include "Attractor.h"
#include "SpringInteraction.h"
#include "Gravity.h"
#include "CoulombInteraction.h"
#include "SoftCollideInteraction.h"
#include <algorithm>
#include <typeindex>
#include "colors.h"
#include "util.h"
#include "SimulatorCommander.h"
#include "SimulatorCommand.h"
#include <memory>
#include <typeindex>
#include <unordered_set>
// #include "draw.cpp"

class MyFrame : public wxFrame {
public:
using CommanderPtr =std::shared_ptr<SimulatorCommander>;
    MyFrame(Simulator& simulator);
    MyFrame(CommanderPtr& commander);

private:
    void OnStart(wxCommandEvent& event); // Event handler for Start button
    void OnStop(wxCommandEvent& event);
    void CreateSetupScreen();
    void StartSimulation();
    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnGravityCheckbox(wxCommandEvent& event);
    void OnSpringCheckbox(wxCommandEvent& event);
    void OnSoftCollideCheckbox(wxCommandEvent& event);
    void OnCoulombCheckbox(wxCommandEvent& event);
    void draw(const Mover& mover, wxBufferedPaintDC& dc, wxColour color);
    bool ValidateInputs();
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void onDoubleClick(wxMouseEvent& event);
    Vect2 mouseToSimulatorPosition();
    

    //GUI elements
    wxPanel* setupPanel;
    wxPanel* simulationPanel;      
    wxSpinCtrl* numMoversCtrl; // Input for number of movers
    wxSpinCtrl* maxSpeedCtrl;  // Input for max speed
    wxSpinCtrl* seedCtrl;
    wxTimer timer;
    wxCheckBox* gravityCheckbox;
    wxCheckBox* springCheckbox;
    wxTextCtrl* gravityParamCtrl;
    wxTextCtrl* springParamKCtrl;
    wxTextCtrl* springParamX0Ctrl;
    wxCheckBox* softCollideCheckbox;
    wxTextCtrl* softCollideParamSpringCtrl;
    wxTextCtrl* softCollideParamRepulsionCtrl;
    wxCheckBox* coulombCheckbox;
    wxTextCtrl* coulombParamKCtrl;

    //color pallete
    std::vector<wxColour> colorPalette = getColorPalette(
        {"#003f5c","#2f4b7c","#665191","#a05195","#d45087","#f95d6a","#ff7c43","#ffa600"}
        );
    //add logger
    
    // functional members
    Simulator& simulator;
    std::shared_ptr<SimulatorCommander> commander;
    Vect2 centeredOn;
    float zoomFactor = 1;
    float maxZoom = 10;
    float minZoom = 0.1;
    std::unordered_set<int> pressedKeys;  // Stores the keys that are currently pressed

};

int X = 800; int Y = 600;
// Constructor
MyFrame::MyFrame(Simulator& simulator)
    : wxFrame(nullptr, wxID_ANY, "Simulation Setup", wxDefaultPosition, wxSize(X,Y),
    wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE), simulator(simulator), 
    commander(std::make_shared<SimulatorCommander>(simulator)), centeredOn(X/2,Y/2)
{
    SetDoubleBuffered(true);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    CreateSetupScreen();
}

MyFrame::MyFrame(CommanderPtr& commander)
    : wxFrame(nullptr, wxID_ANY, "Simulation Setup", wxDefaultPosition, wxSize(X,Y),
    wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE), simulator(commander->simulator), 
    commander(commander), centeredOn(X/2,Y/2)
{
    SetDoubleBuffered(true);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    CreateSetupScreen();
}

// Create the setup screen
void MyFrame::CreateSetupScreen() {
    setupPanel = new wxPanel(this);

    // Layout for setup screen
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // sizer for main info, containing number of movers, max speed, and seed
    wxBoxSizer* mainInfoSizer = new wxBoxSizer(wxHORIZONTAL);
    // choose initial random seed
    wxBoxSizer* seedSizer = new wxBoxSizer(wxVERTICAL); 
    seedSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "Random Seed:"), 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    seedCtrl = new wxSpinCtrl(setupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, -1));
    seedCtrl->SetRange(0, 100000);
    seedSizer->Add(seedCtrl, 0, wxALL, 5);
    mainInfoSizer->Add(seedSizer, 0, wxALL | wxEXPAND, 5);

    // Number of Movers
    wxBoxSizer* moversSizer = new wxBoxSizer(wxVERTICAL);
    moversSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "Number of Movers:"), 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    numMoversCtrl = new wxSpinCtrl(setupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, -1));
    numMoversCtrl->SetRange(1, 10000); // Set range for number of movers
    numMoversCtrl->SetValue(3);
    moversSizer->Add(numMoversCtrl, 0, wxALL, 5);
    mainInfoSizer->Add(moversSizer, 0, wxALL | wxEXPAND, 5);

    // Maximum Speed
    wxBoxSizer* speedSizer = new wxBoxSizer(wxVERTICAL);
    speedSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "Max Initial Speed:"), 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    maxSpeedCtrl = new wxSpinCtrl(setupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, -1));
    maxSpeedCtrl->SetRange(0, 1000); // Set range for speed
    maxSpeedCtrl->SetValue(50);
    speedSizer->Add(maxSpeedCtrl, 0, wxALL, 5);
    mainInfoSizer->Add(speedSizer, 0, wxALL | wxEXPAND, 5);

    //boxsizer that will hold all interactions
    wxBoxSizer* interactionsSizer = new wxBoxSizer(wxVERTICAL);

    // Gravity interaction
    wxStaticBoxSizer* gravitySizer = new wxStaticBoxSizer(wxHORIZONTAL, setupPanel, "Gravity Interaction");
    gravityCheckbox = new wxCheckBox(setupPanel, wxID_ANY, "Enable Gravity");
    gravityParamCtrl = new wxTextCtrl(setupPanel, wxID_ANY, "10000", wxDefaultPosition, wxSize(100, -1));
    gravityParamCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gravityParamCtrl->Enable(false); // Initially disabled
    gravityCheckbox->Bind(wxEVT_CHECKBOX, &MyFrame::OnGravityCheckbox, this);

    gravitySizer->Add(gravityCheckbox, 0, wxALL, 5);
    gravitySizer->Add(new wxStaticText(setupPanel, wxID_ANY, "G:"), 0, wxALL, 5);
    gravitySizer->Add(gravityParamCtrl, 0, wxALL, 5);
    interactionsSizer->Add(gravitySizer, 0, wxALL | wxEXPAND, 5);

    // Spring interaction
    wxStaticBoxSizer* springSizer = new wxStaticBoxSizer(wxHORIZONTAL, setupPanel, "Spring Interaction");
    springCheckbox = new wxCheckBox(setupPanel, wxID_ANY, "Enable Spring");
    springParamKCtrl = new wxTextCtrl(setupPanel, wxID_ANY, "0.5", wxDefaultPosition, wxSize(100, -1));
    springParamX0Ctrl = new wxTextCtrl(setupPanel, wxID_ANY, "250", wxDefaultPosition, wxSize(100, -1));
    springParamKCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    springParamX0Ctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    springParamKCtrl->Enable(false); // Initially disabled
    springParamX0Ctrl->Enable(false); // Initially disabled
    springCheckbox->Bind(wxEVT_CHECKBOX, &MyFrame::OnSpringCheckbox, this);

    springSizer->Add(springCheckbox, 0, wxALL, 5);
    springSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "k:"), 0, wxALL, 5);
    springSizer->Add(springParamKCtrl, 0, wxALL, 5);
    springSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "x0:"), 0, wxALL, 5);
    springSizer->Add(springParamX0Ctrl, 0, wxALL, 5);
    interactionsSizer->Add(springSizer, 0, wxALL | wxEXPAND, 5);

    // Soft Collide interaction
    wxStaticBoxSizer* softCollideSizer = new wxStaticBoxSizer(wxHORIZONTAL, setupPanel, "Soft Collide Interaction");
    softCollideCheckbox = new wxCheckBox(setupPanel, wxID_ANY, "Enable Soft Collide");
    softCollideParamSpringCtrl = new wxTextCtrl(setupPanel, wxID_ANY, "1.0", wxDefaultPosition, wxSize(100, -1));
    softCollideParamRepulsionCtrl = new wxTextCtrl(setupPanel, wxID_ANY, "1.0", wxDefaultPosition, wxSize(100, -1));
    softCollideParamSpringCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    softCollideParamRepulsionCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    softCollideParamSpringCtrl->Enable(false); // Initially disabled
    softCollideParamRepulsionCtrl->Enable(false); // Initially disabled
    softCollideCheckbox->Bind(wxEVT_CHECKBOX, &MyFrame::OnSoftCollideCheckbox, this);

    softCollideSizer->Add(softCollideCheckbox, 0, wxALL, 5);
    softCollideSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "Spring:"), 0, wxALL, 5);
    softCollideSizer->Add(softCollideParamSpringCtrl, 0, wxALL, 5);
    softCollideSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "Repulsion:"), 0, wxALL, 5);
    softCollideSizer->Add(softCollideParamRepulsionCtrl, 0, wxALL, 5);
    interactionsSizer->Add(softCollideSizer, 0, wxALL | wxEXPAND, 5);

    // Coulomb interaction
    wxStaticBoxSizer* coulombSizer = new wxStaticBoxSizer(wxHORIZONTAL, setupPanel, "Coulomb Interaction");
    coulombCheckbox = new wxCheckBox(setupPanel, wxID_ANY, "Enable Coulomb");
    coulombParamKCtrl = new wxTextCtrl(setupPanel, wxID_ANY, "10000", wxDefaultPosition, wxSize(100, -1));
    coulombParamKCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    coulombParamKCtrl->Enable(false); // Initially disabled
    coulombCheckbox->Bind(wxEVT_CHECKBOX, &MyFrame::OnCoulombCheckbox, this);

    coulombSizer->Add(coulombCheckbox, 0, wxALL, 5);
    coulombSizer->Add(new wxStaticText(setupPanel, wxID_ANY, "k:"), 0, wxALL, 5);
    coulombSizer->Add(coulombParamKCtrl, 0, wxALL, 5);
    interactionsSizer->Add(coulombSizer, 0, wxALL | wxEXPAND, 5);

    // Start Button
    wxButton* startButton = new wxButton(setupPanel, wxID_ANY, "Start");
    startButton->Bind(wxEVT_BUTTON, &MyFrame::OnStart, this);
  

    // Add everything to the sizer
    // sizer->Add(moversSizer, 0, wxALL | wxEXPAND, 5);
    // sizer->Add(speedSizer, 0, wxALL | wxEXPAND, 5);
    // sizer->Add(seedSizer, 0, wxALL | wxEXPAND, 5);
    sizer->Add(mainInfoSizer, 0, wxALL | wxEXPAND, 5);
    sizer->Add(interactionsSizer, 0, wxALL | wxEXPAND, 5);
    // sizer->Add(springSizer, 0, wxALL | wxEXPAND, 10);
    // sizer->Add(gravitySizer, 0, wxALL | wxEXPAND, 10);
    // sizer->Add(softCollideSizer, 0, wxALL | wxEXPAND, 10);
    // sizer->Add(coulombSizer, 0, wxALL | wxEXPAND, 10);

    sizer->Add(startButton, 0, wxALL | wxALIGN_CENTER, 10);

    setupPanel->SetSizer(sizer);
}

// Event handler for Start button
void MyFrame::OnStart(wxCommandEvent& event) {

    if (!ValidateInputs()) {
        return;
    }
    // Hide setup panel
    setupPanel->Hide();

    // Launch the simulation
    StartSimulation();
}

// Launch the simulation
void MyFrame::StartSimulation() {

    // Create a timer for updates
    timer.SetOwner(this);
    timer.Start(1000 / 30); // 30 frames per second

    // Bind paint event to draw
    Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);
    Bind(wxEVT_KEY_DOWN, &MyFrame::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &MyFrame::OnKeyUp, this);
    Bind(wxEVT_MOUSEWHEEL, &MyFrame::OnMouseWheel, this);
    Bind(wxEVT_LEFT_DCLICK, &MyFrame::onDoubleClick, this);

    // Bind timer event to update logic
    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
    commander->runSimulator();
}


void MyFrame::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(this);
    // Clear the screen
    dc.SetBackground(wxBrush(wxColour(wxString("BLACK"))));
    dc.Clear();
    //setting viewport based on center and zoom
    zoomFactor = std::clamp(zoomFactor, minZoom, maxZoom);
    dc.SetUserScale(zoomFactor, zoomFactor);
    dc.SetDeviceOrigin(centeredOn.x, centeredOn.y);   
    // Draw the circles
    for (auto& mover : simulator.movers) {
        int colorIndex = (mover->id) % colorPalette.size();
        wxColour color = colorPalette[colorIndex];
        draw(*mover, dc, color);
    }

}

void MyFrame::OnTimer(wxTimerEvent& event) {
    // Update the circle's position
    // wxLogMessage("Mover Position - X: %d, Y: %d", simulator.movers[0]->position.x, simulator.movers[0]->position.y); 
    commander->update();
    // wxLogMessage("Mover Position - X: %d, Y: %d", simulator.movers[0]->position.x, simulator.movers[0]->position.y); 
    // std::cout << "Mover1: " << simulator.movers[0]->position << std::endl;
    // std::cout << "Mover2: " << simulator.movers[1]->position << std::endl;
    // Update the circle's position

    // Refresh the window to trigger a repaint
    Refresh();
}

void MyFrame::OnStop(wxCommandEvent& event) { //stop timer and return to setup screen
    timer.Stop();
    simulationPanel->Hide();
    setupPanel->Show();
    Layout();
    //reset viewport
    centeredOn = Vect2(X/2, Y/2);
    zoomFactor = 1;
}

void MyFrame::OnGravityCheckbox(wxCommandEvent& event) {
        // Enable or disable the gravity parameter based on checkbox state
        gravityParamCtrl->Enable(gravityCheckbox->IsChecked());
  }

void MyFrame::OnSpringCheckbox(wxCommandEvent& event) {
    // Enable or disable the spring parameters based on checkbox state
    bool isChecked = springCheckbox->IsChecked();
    springParamKCtrl->Enable(isChecked);
    springParamX0Ctrl->Enable(isChecked);
}
void MyFrame::OnSoftCollideCheckbox(wxCommandEvent& event) {
    // Enable or disable the soft collide parameters based on checkbox state
    bool isChecked = softCollideCheckbox->IsChecked();
    softCollideParamSpringCtrl->Enable(isChecked);
    softCollideParamRepulsionCtrl->Enable(isChecked);
}

void MyFrame::OnCoulombCheckbox(wxCommandEvent& event) {
    // Enable or disable the coulomb parameters based on checkbox state
    //refactor so event knows which checkbox was checked
    bool isChecked = event.IsChecked();
    // bool isChecked = coulombCheckbox->IsChecked();
    coulombParamKCtrl->Enable(isChecked);
}

bool MyFrame::ValidateInputs() {
  // check that text fields can be converted to floats. if not, pop up wxMessage box and quit out of function early
  try {
    float k = std::stof(springParamKCtrl->GetValue().ToStdString());
    float x0 = std::stof(springParamX0Ctrl->GetValue().ToStdString());
    float G = std::stof(gravityParamCtrl->GetValue().ToStdString());
  } catch (std::invalid_argument& e) {
    wxMessageBox("Invalid input. Please enter a valid number.", "Error", wxOK | wxICON_ERROR);
    return false;
  } catch (std::out_of_range& e) {
    wxMessageBox("Out of range. Please try a smaller number.", "Error", wxOK | wxICON_ERROR);
    return false;
  }
  return true;
}

void MyFrame::draw(const Mover& mover, wxBufferedPaintDC& dc, wxColor color) {

  dc.SetBrush(color);
//   dc.SetBrush(*wxBLUE_BRUSH);
  dc.SetPen(*wxTRANSPARENT_PEN);
  // float x = mover.position.x - centeredOn.x;
  // float y = mover.position.y - centeredOn.y;
  dc.DrawCircle(mover.position.x, mover.position.y, mover.radius);
}

void MyFrame::OnKeyDown(wxKeyEvent& event) {

  //up arrow key
  int speed = 10;
  if (event.GetKeyCode() == wxKeyCode::WXK_UP) {
    centeredOn.y += 10;
    Refresh();
    return;
  }
  //down arrow key
  if (event.GetKeyCode() == wxKeyCode::WXK_DOWN) {
    centeredOn.y -= 10;
    Refresh();
    return;
  }
  //left arrow key
  if (event.GetKeyCode() == wxKeyCode::WXK_LEFT) {
    centeredOn.x += 10;
    Refresh();
    return;
  }
  //right arrow key
  if (event.GetKeyCode() == wxKeyCode::WXK_RIGHT) {
    centeredOn.x -= 10;
    Refresh();
    return;
  }
  //zoom in with +
  if (event.GetKeyCode() == '+') {
      zoomFactor *= 2; // Zoom in
      Refresh(); // Trigger a repaint with the new zoom factor
      return;
  }
  //zoom out with -
  if (event.GetKeyCode() == '-') {
      zoomFactor /= 2; // Zoom out
      Refresh(); // Trigger a repaint with the new zoom factor
      return;
  }

  if (event.GetKeyCode() == 'm' || event.GetKeyCode() == 'M') { //add mover
    // wxLogMessage("m key pressed");
    //if already pressed, skip
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
        pressedKeys.insert(event.GetKeyCode());
      }
    else return;
    Vect2 newPosition = mouseToSimulatorPosition();
    MoverArgs args = MoverArgs(newPosition, Vect2(0,0), Vect2(0,0), 20.0, 20.0);
    commander->addCommandAddMover(typeid(NewtMover), args);
    //wxlog creation of mover at position. format for float
    // wxLogMessage("Mover created at position - X: %.0f, Y: %.0f", newPosition.x, newPosition.y);
    Refresh();

    return;
  }
  //press space to pause
  if (event.GetKeyCode() == ' ') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    }
    else return;
    commander->togglePause();
    return;
  }
  //press 's' to step one
  if (event.GetKeyCode() == 's' || event.GetKeyCode() == 'S') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    }
    else return;
    commander->addCommandStep();
    return;
  }
  //press r to remove movers at mouse position
  if (event.GetKeyCode() == 'r' || event.GetKeyCode() == 'R') { 
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    }
    else return;
    Vect2 clickedPosition = mouseToSimulatorPosition();
    std::vector<int> ids = SimulatorCommand::getMoverIdsByPosition(commander->simulator, clickedPosition);
    for (auto& id : ids) {
        //if shift+r, delete group (if it is a group)
        auto moverPtr = (commander->simulator.find_mover(id));
        bool isGroup = dynamic_cast<RigidConnectedMover*>(moverPtr->get()) != nullptr;
        if (event.ShiftDown() && isGroup) {
                // wxLogMessage("GroupDeletion");
                commander->addCommandDeleteGroup(id);
        }
        else //just delete the one mover
            // wxLogMessage("SingleDeletion");
            commander->addCommandDeleteMover(id);
    }
    return;
  }
  //n begins creation of a new group, and g will add a mover to that group.
  //n again will stop adding to current group
  //n begins creation of a new group, and g will add a mover to that group.
  //n again will stop adding to current group
  static bool addingToGroup = false;
  static std::vector<int> moverIdsForGrouping;
  if (event.GetKeyCode() == 'n' || event.GetKeyCode() == 'N') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
        pressedKeys.insert(event.GetKeyCode());
      } else return;
    addingToGroup = !addingToGroup;
    if (!addingToGroup) { //group done, create group
        commander->addCommandCreateGroup(moverIdsForGrouping);
        moverIdsForGrouping.clear();
    }
  }
  
  if (event.GetKeyCode() == 'g' || event.GetKeyCode() == 'G') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    } else return;
    if (addingToGroup) {
        Vect2 mousePosition = mouseToSimulatorPosition();
        std::vector<int> ids = SimulatorCommand::getMoverIdsByPosition(commander->simulator, mousePosition);
        for (auto& id : ids) {
            moverIdsForGrouping.push_back(id);
        }
    }
  }

  //'u' to ungroup a group of movers
  if (event.GetKeyCode() == 'u' || event.GetKeyCode() == 'U') {
   if (pressedKeys.count(event.GetKeyCode()) == 0) {
     pressedKeys.insert(event.GetKeyCode());
   } else return;
       Vect2 mousePosition = mouseToSimulatorPosition();
       std::vector<int> ids = SimulatorCommand::getMoverIdsByPosition(commander->simulator, mousePosition);
       for (auto& id : ids) {
           //ungroup associated group
           commander->addCommandUngroup(id);
       }

  }
  //'k' kicks movers moused over
  if (event.GetKeyCode() == 'k' || event.GetKeyCode() == 'K') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    } else return;
    Vect2 mousePosition = mouseToSimulatorPosition();
    std::vector<int> ids = SimulatorCommand::getMoverIdsByPosition(commander->simulator, mousePosition);
    //define kick lambda
    auto kickLambda = [mousePosition](Mover& mover) {
        Vect2 r = mover.position - mousePosition;
        float d = mover.mass*(r.mag());
        float kickStrength = 50;
        Vect2 force = kickStrength*d *r/r.mag();
        mover.apply_force(force);
      };
    for (auto& id : ids) {
      commander->addCommandAffectMover(kickLambda, id);
    }
  }
  static std::vector<int> moverIdsMoving;
  //0 press adds to moving group
  if (event.GetKeyCode() == '0') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    } else return;
    Vect2 mousePosition = mouseToSimulatorPosition();
    std::vector<int> ids = SimulatorCommand::getMoverIdsByPosition(commander->simulator, mousePosition);
    for (auto& id : ids) {
      auto it = std::find(moverIdsMoving.begin(), moverIdsMoving.end(), id);
      if (it == moverIdsMoving.end()) { //not found, add
        moverIdsMoving.push_back(id);
      }
      else {
        moverIdsMoving.erase(it); //found, remove
      }
    }
  }
  //4 to set velocity of movers in group
  if (event.GetKeyCode() =='4') {
    Vect2 velocity = {-1000.0f, 0.0f};
    auto setVelocityLambda = [velocity](Mover& mover) { mover.velocity = velocity;};
    for (auto id : moverIdsMoving) {
      commander->addCommandAffectMover(setVelocityLambda, id);
    }
  }


  //1 adds a spring interaction
  if (event.GetKeyCode() == '1') {
    if (pressedKeys.count(event.GetKeyCode()) == 0) {
      pressedKeys.insert(event.GetKeyCode());
    }
    else return;
    commander->addCommandAddSpring(1, 20); 
  }
}

void MyFrame::OnKeyUp(wxKeyEvent& event) {
  pressedKeys.erase(event.GetKeyCode());
  event.Skip();
}

void MyFrame::OnMouseWheel(wxMouseEvent& event) {
    // Get the current logical position of the screen center
    Vect2 logicalCenter((X/2 - centeredOn.x) / zoomFactor, (Y/2 - centeredOn.y) / zoomFactor);

    // Adjust zoom factor
    float newZoom;
    if (event.GetWheelRotation() > 0) {
        newZoom = zoomFactor * 1.1; // Zoom in
    } else {
        newZoom = zoomFactor / 1.1; // Zoom out
    }
    zoomFactor = std::clamp(newZoom, minZoom, maxZoom);
    // Compute the new centeredOn value so the center stays fixed
    centeredOn.x = X/2 - logicalCenter.x * zoomFactor;
    centeredOn.y = Y/2 - logicalCenter.y * zoomFactor;

    Refresh(); // Trigger a repaint with the new zoom factor
}

void MyFrame::onDoubleClick(wxMouseEvent& event) {
    // set centeredOn to the mouse position
    Vect2 clickedPosition = Vect2(event.GetX() - X/2, event.GetY() - Y/2);
    centeredOn = centeredOn - clickedPosition; 
    Refresh();
};

Vect2 MyFrame::mouseToSimulatorPosition() {
    wxPoint mousePos = wxGetMousePosition();
    mousePos = ScreenToClient(mousePos);
    Vect2 clickedPosition = Vect2(mousePos.x, mousePos.y);
    // wxLogMessage("mouse position - x: %d, y: %d", mousePos.x, mousePos.y);
    //convert to simulation coordinates, accounting for center and zoom
    clickedPosition = (clickedPosition - centeredOn)/zoomFactor;
    return clickedPosition;
};