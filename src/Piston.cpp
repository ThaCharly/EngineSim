#include "Piston.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244065664308602139494639522473719070217986094370277053921717629317675238467481846766940513200056812714526356082778577134275778960917363717872146844090122495343014654958537105079227968925892354201995611212902196086403441815981362977477130996051870721134999999837297804995105973173281609631859502445945534690830264252230825334468503526193118817101100313388246468157082630100594858704003186480342194897278290641045072636881313739855256117322040245091227700226941127573627280495738108967504018369868368450725799364729060762996941380475654823728997180326802474420629269124859052181004459842150591120249441341728531478105803860326071847891885627529465490681131715993432359734949850904452444873330391072253837421821408835086569637893063936

#endif

Piston::Piston(float x, float y) 
    : crankCenter(x, y), crankRadius(50.f), rodLength(140.f) 
{
    // 1. Configurar el centro del cigüeñal (Main Bearing)
    mainBearing.setRadius(15.f);
    mainBearing.setOrigin(15.f, 15.f);
    mainBearing.setPosition(crankCenter);
    mainBearing.setFillColor(sf::Color(80, 80, 80)); // Gris oscuro

    // 2. Configurar brazo del cigüeñal (Crank Arm)
    crankArm.setSize(sf::Vector2f(crankRadius, 20.f));
    crankArm.setOrigin(0.f, 10.f); // Pivota desde el centro
    crankArm.setPosition(crankCenter);
    crankArm.setFillColor(sf::Color(100, 100, 100));

    // 3. Configurar la muñequilla (Crank Pin)
    crankPin.setRadius(10.f);
    crankPin.setOrigin(10.f, 10.f);
    crankPin.setFillColor(sf::Color(50, 50, 50));

    // 4. Configurar la Biela (Connecting Rod)
    // La haremos un poco más larga visualmente para que solape bien
    pistonRod.setSize(sf::Vector2f(10.f, rodLength + 10.f)); 
    pistonRod.setOrigin(5.f, 0.f); // Pivota desde arriba (unión con pistón)
    pistonRod.setFillColor(sf::Color(160, 160, 160)); // Acero

    // 5. Configurar la Cabeza del Pistón
    pistonHead.setSize(sf::Vector2f(60.f, 50.f));
    pistonHead.setOrigin(30.f, 25.f); // Centro del pistón
    pistonHead.setFillColor(sf::Color(200, 200, 200)); // Aluminio
    pistonHead.setOutlineThickness(2.f);
    pistonHead.setOutlineColor(sf::Color(50, 50, 50));

    // 6. Bulón (Wrist Pin)
    wristPin.setRadius(6.f);
    wristPin.setOrigin(6.f, 6.f);
    wristPin.setFillColor(sf::Color(50, 50, 50));

    // 7. Paredes del cilindro (Líneas estáticas)
    cylinderWalls.setPrimitiveType(sf::Lines);
    // Pared izquierda
    cylinderWalls.append(sf::Vertex(sf::Vector2f(x - 32.f, y - crankRadius - rodLength - 60.f), sf::Color::White));
    cylinderWalls.append(sf::Vertex(sf::Vector2f(x - 32.f, y - 50.f), sf::Color::White));
    // Pared derecha
    cylinderWalls.append(sf::Vertex(sf::Vector2f(x + 32.f, y - crankRadius - rodLength - 60.f), sf::Color::White));
    cylinderWalls.append(sf::Vertex(sf::Vector2f(x + 32.f, y - 50.f), sf::Color::White));
}

void Piston::update(float angle) {
    // 1. Calcular posición de la muñequilla (Crank Pin)
    // Usamos coordenadas polares estándar.
    // Nota: Si quieres que angle=0 sea "Arriba" (TDC), usa sin(a) para X y -cos(a) para Y.
    // Pero para simplificar, usaremos el estándar matemático (0 = Derecha):
    float crankX = crankCenter.x + crankRadius * std::cos(angle);
    float crankY = crankCenter.y + crankRadius * std::sin(angle);
    sf::Vector2f crankPos(crankX, crankY);

    // 2. Calcular posición del Pistón
    // Pitágoras: La biela (rod) es la hipotenusa, la distancia X es un cateto.
    float dx = crankX - crankCenter.x;
    float diffX = std::abs(dx);
    
    // Altura vertical desde la muñequilla hasta el pistón
    float rodVerticalH = std::sqrt(rodLength * rodLength - diffX * diffX);
    
    // La posición Y del pistón es la Y de la muñequilla MENOS la altura vertical (porque sube)
    sf::Vector2f pistonPos(crankCenter.x, crankY - rodVerticalH);


    // --- ACTUALIZAR VISUALES ---

    // A. Muñequilla (La bolita gris oscura)
    crankPin.setPosition(crankPos);

    // B. Brazo del cigüeñal (El rectángulo gris oscuro)
    // CORRECCIÓN CLAVE: No uses 'angle' directo. Calcula el ángulo real del vector.
    sf::Vector2f delta = crankPos - crankCenter;
    float armRotation = std::atan2(delta.y, delta.x) * 180.f / M_PI;
    
    crankArm.setPosition(crankCenter);
    crankArm.setRotation(armRotation); 

    // C. Pistón y Bulón
    pistonHead.setPosition(pistonPos);
    wristPin.setPosition(pistonPos);

    // D. Biela (El rectángulo largo)
    // Misma lógica: calculamos el ángulo entre el pistón y la muñequilla
    pistonRod.setPosition(pistonPos); // Anclada arriba
    
    sf::Vector2f rodDelta = crankPos - pistonPos; // Vector desde el pistón hacia la muñequilla
    float rodAngle = std::atan2(rodDelta.y, rodDelta.x) * 180.f / M_PI;
    
    // Restamos 90 grados porque el rectángulo de SFML suele estar horizontal por defecto
    // o ajustamos según tu 'setOrigin'. 
    // Si tu biela es vertical (height > width), el ángulo base es -90 offset.
    // Asumiendo que definiste la biela vertical:
    pistonRod.setRotation(rodAngle - 90.f); 
}

void Piston::draw(sf::RenderWindow& window) {
    window.draw(cylinderWalls); // Dibujar paredes detrás
    window.draw(pistonRod);     // Biela detrás del pistón
    window.draw(crankArm);
    window.draw(mainBearing);
    window.draw(pistonHead);
    window.draw(wristPin);
    window.draw(crankPin);      // Muñequilla encima de la biela
}