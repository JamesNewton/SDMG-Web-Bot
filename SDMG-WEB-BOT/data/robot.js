/* eslint-disable */

// define devices and controls //
var arcadeDrive, motorMotor;
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
}

const tooClose = 15
var aware = undefined 
var playBack = ""
var interval = 0
var lastime = 0

// this is run at update rate //
function loop () {
  //var infoBox = document.getElementById('info-box')
  var dist = infoBox.botStatus.cm
  if (dist && !aware) aware = setInterval(fireUpdate,100) //even when not being touched, do occasional updates, which calls the loop
  if (dist === undefined || dist < 3 || dist > 200) dist = 200
  var avoid = (tooClose - dist) * 0.02
  if (avoid < 0) avoid = 0
  if (dist < tooClose) console.log(avoid, driveStick.x)
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
