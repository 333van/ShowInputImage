param(
  [String]$action="build"
)

function main(){
  if($action -eq "build")    { Start-BuildRoutinue; }
  elseif($action -eq "test") { Start-BuildRoutinue; Start-TestRoutinue; }
}

function Start-BuildRoutinue($outputPath="sii"){
  gcc main.c -o $outputPath
}

function Start-TestRoutinue($inputImagePath="./input.jpg"){
  ./sii.exe $inputImagePath
}

main
