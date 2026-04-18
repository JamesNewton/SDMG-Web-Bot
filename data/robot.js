/* eslint-disable */

// define devices and controls //
var arcadeDrive, weaponMotor;
var driveStick, weaponSlider, forwardButton, reverseButton;
var recordButton;

// this is run once //
function setup () {
  // setup hardware interface //
  arcadeDrive = new ArcadeDrive()
  weaponMotor = new Motor('weaponMotor')

  // create the drive joystick //
  driveStick = new Joystick('drive')
  driveStick.position.x = 20  // positions + sizes in % of screen size //
  driveStick.position.y = 50
  driveStick.radius = 15

  // weapon power //
  weaponSlider = new Slider('weapon')
  weaponSlider.position.x = 85
  weaponSlider.position.y = 20
  weaponSlider.radius = 5
  weaponSlider.length = 30
  weaponSlider.type = Slider.VERTICAL
  weaponSlider.sticky = true
  weaponSlider.style = 'red'

  // drive direction buttons //
  reverseButton = new Button('reverse')
  reverseButton.position.x = 60
  reverseButton.position.y = 40
  reverseButton.radius = 5
  reverseButton.sticky = true
  reverseButton.style = 'blue'
  reverseButton.groupName = 'weaponGroup'

  forwardButton = new Button('forward')
  forwardButton.position.x = 60
  forwardButton.position.y = 70
  forwardButton.radius = 5
  forwardButton.sticky = true
  forwardButton.style = 'blue'
  forwardButton.groupName = 'weaponGroup'
  forwardButton.pressed = true

  recordButton = new Button('record')
  recordButton.position.x = 60
  recordButton.position.y = 10
  recordButton.radius = 4
  recordButton.sticky = true
  recordButton.style = 'red'
  recordButton.pressed = false
  //recordButton.groupName = 'recordGroup'

  infoBox.botStatus = {}

  // --- THE SMART HEARTBEAT ---
  // Fires an update every 100ms ONLY if the controls are active or a sensor is reading
  setInterval(function() {
    // Check if the joystick is off-center (give it a tiny 1% deadzone)
    var isDriving = Math.abs(driveStick.x) > 0.01 || Math.abs(driveStick.y) > 0.01;
    // Check if the weapon is spinning
    var isWeaponActive = Math.abs(weaponSlider.value) > 0.01;
    // Check if we have a valid, close-range sensor reading that needs monitoring
    var hasValidSensor = infoBox.botStatus.cm !== undefined && infoBox.botStatus.cm > 0 && infoBox.botStatus.cm < 200;

    if (isDriving || isWeaponActive || hasValidSensor) {
      fireUpdate();
    }
  }, 100);
}

const tooClose = 15
var playBack = ""
var interval = 0
var lastime = 0

// this is run at update rate //
function loop () {
  //var infoBox = document.getElementById('info-box')
  var dist = infoBox.botStatus.cm
  var avoid = 0
  
  // Only calculate avoidance if the sensor reading is valid and dangerously close
  if (dist !== undefined && dist > 3 && dist < tooClose) {
    avoid = (tooClose - dist) * 0.02
    console.log("Avoiding obstacle:", avoid, driveStick.x)
  }

  // handle driving //
  var speed = driveStick.y
  var rotation = driveStick.x + avoid
  arcadeDrive.setSpeedAndRotation(speed, rotation)
  
  // handle weapon control //
  weaponMotor.set(+weaponSlider.value)

  // handle driving reverse //
  if (reverseButton.pressed) {
    arcadeDrive.leftMotor.reversed = true
    arcadeDrive.rightMotor.reversed = true
    arcadeDrive.swapMotors = true
  } else {
    arcadeDrive.leftMotor.reversed = false
    arcadeDrive.rightMotor.reversed = false
    arcadeDrive.swapMotors = false
  }
  
  if (recordButton.pressed) {
    if (!playBack) { 
      lastime = Date.now() 
      playBack = "500"
      let savedata = document.getElementById("save-data")
      savedata.style.backgroundColor = "#ffcccc"
    }
    interval = Date.now() - lastime
    if (interval>50) {
      //let l = Math.floor(driveStick.x * 1023)
      //let r = Math.floor(driveStick.y * 1023)
      //let w = Math.floor(weaponSlider.value * 1023)
      //playBack+="["+interval+","+l+":"+r+":"+w+"]\n"
      playBack+="\n, ["+interval+","+getPacket(HardwareManager.getOutputs())+"]"
      interval = Date.now() - lastime
      lastime = Date.now()
    }
  } else {
    if (playBack) {
      playBack = "[" + playBack + "\n]\n"
      console.log(playBack)
      let savedata = document.getElementById("save-data")
      savedata.value = playBack
      playBack = ""
    }
  }
}
