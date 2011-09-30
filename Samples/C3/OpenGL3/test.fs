#version 330
#extension GL_EXT_gpu_shader4 : enable
out vec4 outputColor;
out vec4 frag_color;
in vec2 FragCoord;
uniform float time;

vec2 resolution = vec2(800,600);
const float PI = 3.14159265;



// Donne la distanc a une metaball de centre center pour un point de position pos
// D'apres wikipedia anglais
float metaball(vec3 center, vec3 pos){
return 1.f/(pow((pos.x - center.x),2) + pow((pos.y - center.y),2) + pow((pos.z - center.z),2));
}

vec4 metaballs(){
/////////////////////////////////////////////////
// Calcul de la position 3D Metaballs
/////////////////////////////////////////////////
//!\\ Les coordonnees pour les metaballs et le tunnels ne sont pas exprimees dans le meme repere

  // Position de la camera dans l'espace
  vec3 posEye = vec3(0.f,0.f,-2.f);
  // Position du pixel dans l'espace
  vec3 posPixel = vec3(1.f-2.f * FragCoord.x / resolution.x, 1.F - 2.f * FragCoord.y / resolution.y,0.f); // Position du pixel sur la grille entre -1 et 1

  // Definition du rayon
  vec3 rayDir = posPixel - posEye; // Vecteur direction du rayon
  rayDir = normalize(rayDir); // Il doit etre normalise pour savoir de quelle distance on avance quand on parcourt le rayon
  vec3 rayPos = posPixel; // Position courante sur le rayon

  // Definition des spheres
  float speed1 = sin(3.f * time);
  float speed1d = sin(3.f * time + PI/2);
  float speed2 = sin(5.f * time);
  float speed2d = sin(5.f * time + PI/2);
  float speed3 = sin(6.f * time);
  float speed3d = sin(6.f * time + PI/2);

  vec3 posSphereR = vec3(1.f + 1.5f * speed2,0.f + speed2,15.f + speed1);
  vec3 posSphereG = vec3(0.5 * speed2,-1.2f + 2.f * speed3,15.f+speed2d);
  vec3 posSphereB = vec3(-1.f + 1.2f * speed2d + 0.5*speed2,0.f,15.f);


  float radius = 1.f; // Meme rayon pour toutes les spheres

  // Si la distance a la sphere est inferieure a error, on considere qu'il y a eu intersection
  float error = 0.25;

  // Pour eviter d'avancer a l'infini
   int maxStep = 16;

  float done = 0.f; // Pour savoir si on a rencontre la sphere

  for (int i = 0; i < maxStep; ++i)
  {
    // Calcul du pas courant selon Iq : le pas courant est la distance a l'objet le plus proche
    float currStep = min(min(distance(rayPos,posSphereR),distance(rayPos,posSphereG)),distance(rayPos,posSphereB)) - radius;

    float mbr = metaball(posSphereR,rayPos);
    float mbg = metaball(posSphereG,rayPos);
    float mbb = metaball(posSphereB,rayPos);
    float value = mbr + mbg + mbb;
    if (value > radius){
      done = 1.f;
      break;
    }

    rayPos += currStep * rayDir; // On avance sur le rayon
  }

  // Calcul de la normale pour l'affichage
  float dr = distance(rayPos,posSphereR) - radius;
  float dg = distance(rayPos,posSphereG) - radius;
  float db = distance(rayPos,posSphereB) - radius;

  float sum = dr + dg + db;

  float alpha = dr / sum;
  float beta = dg / sum;
  float gamma = db / sum;

  vec3 barycentre = alpha * posSphereR + beta * posSphereG + gamma * posSphereB;
  vec3 normal = rayPos - barycentre;
  normal = normalize(normal);

  vec4 sphere_color = vec4(1.f - alpha, 1.f - beta, 1.f - gamma, done);
  //return sphere_color;

/////////////////////////////////////////////////
// Ajout d'une lampe (meme que pour le tunnel mais position differente)
/////////////////////////////////////////////////  

  // Parametres de la lampe
  float speed11 = cos(4.f * time);
  float speed12 = sin(4.f * time);
  vec3 lp = vec3(2*speed11,2*speed12,15.f); // lightPosition en haut a droite de la sphere
//  vec3 lp = vec3(1.f+speed1,-1.f+speed3,15.f); // lightPosition en haut a droite de la sphere
  vec3 lc = vec3(1.f, 1.f, 1.f); // lightColor

  vec3 pos = rayPos;
  vec3 toLight = lp - pos;
  float dtl = distance(lp,pos); // distance to light

  toLight = normalize(toLight);
  float coeff = dot(toLight,normal);
  coeff = max(coeff, 0.5f); // Au cas ou la lampe sort du volume

  // Influence de la lumiere
  vec3 li = 5.f * lc * coeff / (dtl * dtl);

  sphere_color = vec4(li.x *sphere_color.r, li.y * sphere_color.g, li.z *sphere_color.b, done);
// Ici sphere_color permet de tracer les metaballs completement
  //return vec4(0.f, 0.f, 0.f, 0.f);
  return sphere_color;
}

vec4 tunnel(){
/////////////////////////////////////////////////
// Calcul de la position 3D Tunnel
/////////////////////////////////////////////////  

  // Coordonnees du point courant entre -1 et 1
  vec2 coord = vec2(1.F-2.f * gl_FragCoord.x / resolution.x, 1.F - 2.f * gl_FragCoord.y / resolution.y);

  // Calcul de la vue
  vec2 origine = vec2(sin(time)/2.f, sin(time+PI/2.f)/2.f);
  float ar = resolution.x/resolution.y; // Aspect ratio
  vec3 view = vec3(ar * (coord.x+origine.x), coord.y + origine.y, 1.f);
  view = normalize(view);

  // Calcul de la distance
  float r = 0.05f; // rayon du cylindre
  float d = r / (1.f - view.z);
  vec3 pos = d * view; // position

  // Calcul de l'angle pour mapper la texture
  float a = 1.f + atan(view.y, view.x) / PI; // Angle
  vec2 UV = vec2(mod((1.f*a),1.f),mod(10.f*d,1.f));

  // Calcul de la normale a la surface
  vec3 normal = vec3(-view.xy,0.f);
  normal = normalize(normal);

   float temp = mod(10*d,2.f);
   float R = mod(floor((a+d)*10.f+time*5.f),2);
   float G = 0.f;
   float B = 0.5f;
   vec4 tunnel_color = vec4(R,G,B,1.0f);  

/////////////////////////////////////////////////
// Ajout d'une lampe tunnel
/////////////////////////////////////////////////  

  // Parametres de la lampe
  vec3 lp = vec3(0.3f, -0.2f, 0.7f); // lightPosition
  vec3 lc = vec3(0.6f, 0.6f, 0.6f); // lightColor

  vec3 toLight = lp - pos;
  float dtl = distance(lp,pos); // distance to light

  toLight = normalize(toLight);
  float coeff = dot(toLight,normal);
  coeff = max(coeff, 0.5f); // Au cas ou la lampe sort du volume

  // Influence de la lumiere
  vec3 li = lc * coeff / (dtl * dtl);
  tunnel_color.xyz *= li;

// Ici tunnel_color  permet de dessiner le tunnel completement
   return tunnel_color;
}

vec4 lightBulb(){
/////////////////////////////////////////////////
// Calcul de la position 
/////////////////////////////////////////////////
//!\\ Les coordonnees pour les metaballs et le tunnels ne sont pas exprimees dans le meme repere

  // Position de la camera dans l'espace
  vec3 posEye = vec3(0.f,0.f,-2.f);
  // Position du pixel dans l'espace
  vec3 posPixel = vec3(1.f-2.f * gl_FragCoord.x / resolution.x, 1.F - 2.f * gl_FragCoord.y / resolution.y,0.f); // Position du pixel sur la grille entre -1 et 1

  // Definition du rayon
  vec3 rayDir = posPixel - posEye; // Vecteur direction du rayon
  rayDir = normalize(rayDir); // Il doit etre normalise pour savoir de quelle distance on avance quand on parcourt le rayon
  vec3 rayPos = posPixel; // Position courante sur le rayon


  float speed1 = cos(4.f * time);
  float speed2 = sin(4.f * time);
  vec3 posSphere = vec3(2*speed1,2*speed2,15.f); // lightPosition en haut a droite de la sphere

  float radius = 0.15f; // Meme rayon pour toutes les spheres

  // Si la distance a la sphere est inferieure a error, on considere qu'il y a eu intersection
  float error = 0.25;

  // Pour eviter d'avancer a l'infini
   int maxStep = 16;

  float done = 0.f; // Pour savoir si on a rencontre la sphere

  for (int i = 0; i < maxStep; ++i)
  {
    // Calcul du pas courant selon Iq : le pas courant est la distance a l'objet le plus proche
    float currStep = distance(rayPos,posSphere) - radius;

    if (currStep < radius){
      done = 1.f;
      break;
    }

    rayPos += currStep * rayDir; // On avance sur le rayon
  }

  vec4 sphere_color = done * vec4(1.f, 1.f, 1.f,1.f) + (1.f - done) * vec4(0.f,0.f,0.f,1.f);
  return sphere_color;
}



void main(){
// Si les metaballs sont visibles, on trace les metaballs, sinon, le tunnel
   vec4 balls = metaballs();
   vec4 tunnel = tunnel();
   frag_color.xyz = balls.a * balls.xyz + (1.f - balls.a) * tunnel.xyz; // Calcul de la couleur du pixel	
   frag_color.a = 1.f;
   vec4 light = lightBulb();
  // frag_color = max(frag_color, light);
}