/*
 * Tweaked code from
 * http://threejs.org/examples/webgl_interactive_voxelpainter.html.
 * 
 * Random walk in 3D with no culture.
 */
if (!Detector.webgl)
    Detector.addGetWebGLMessage();

var container, jsContainer, stats;
var camera, scene, renderer;
var projector, plane, cube;
var mouse2D, mouse3D, raycaster,
        rollOveredFace, isMouseDown = false,
        theta = 45 * 0.5;

var rollOverMesh, rollOverMaterial;
var voxelPosition = new THREE.Vector3(), tmpVec = new THREE.Vector3(), normalMatrix = new THREE.Matrix3();
var cubeGeo, cubeMaterial;
var i, intersector;
var scaling = 1;

var vx = 0, vy = 0, vz = 0, cCount = 0;
window.setInterval(function() {
    if(cCount++ > 1000) return;
    addVoxel(vx, vy, vz);
    var r = Math.random();

    vx += (r < 0.33) ? -1 :
            (r < 0.66) ? 0 : 1;

    r = Math.random();
    vy += (r < 0.33) ? -1 :
            (r < 0.66) ? 0 : 1;
    r = Math.random();
    vz += (r < 0.33) ? -1 :
            (r < 0.66) ? 0 : 1;
    
    var min = -10;
    var max = 10;
    if(vx > max) vx = min; else if(vx < min) vx = max;
    if(vy > max) vy = min; else if(vy < min) vy = max;
    if(vz > max) vz = min; else if(vz < min) vz = max;
    

}, 100);

init();
//animate();

function addVoxel(x, y, z) {
    var position = new THREE.Vector3(x * 50, y * 50, z * 50);
    var voxel = new THREE.Mesh(cubeGeo, cubeMaterial);
    voxel.position.copy(position);
    voxel.matrixAutoUpdate = false;
    voxel.updateMatrix();
    scene.add(voxel);
}

function init() {

    container = document.getElementById('map-widget-div');
    jqContainer = $(container);
    jqContainer.css('background-color', '#ffcccc');
    jqContainer.height(200);
    jqContainer.width(400);


    camera = new THREE.PerspectiveCamera(45, jqContainer.width() / jqContainer.height(), 1, 10000);
    camera.position.y = 1400;

    scene = new THREE.Scene();

    // roll-over helpers

    rollOverGeo = new THREE.CubeGeometry(50, 50, 50);
    rollOverMaterial = new THREE.MeshBasicMaterial({color: 0xff0000, opacity: 0.5, transparent: true});
    rollOverMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
    scene.add(rollOverMesh);

    // cubes

    cubeGeo = new THREE.CubeGeometry(50, 50, 50);
    cubeMaterial = new THREE.MeshLambertMaterial({color: 0xfeb74c, ambient: 0x00ff80, shading: THREE.FlatShading/*, map: THREE.ImageUtils.loadTexture("textures/square-outline-textured.png")*/});
    cubeMaterial.ambient = cubeMaterial.color;

    // picking

    projector = new THREE.Projector();

    // grid

    var size = 500, step = 50;

    var geometry = new THREE.Geometry();

    for (var i = -size; i <= size; i += step) {

        geometry.vertices.push(new THREE.Vector3(-size, 0, i));
        geometry.vertices.push(new THREE.Vector3(size, 0, i));

        geometry.vertices.push(new THREE.Vector3(i, 0, -size));
        geometry.vertices.push(new THREE.Vector3(i, 0, size));

    }

    var material = new THREE.LineBasicMaterial({color: 0x000000, opacity: 0.2});

    var line = new THREE.Line(geometry, material);
    line.type = THREE.LinePieces;
    scene.add(line);

    plane = new THREE.Mesh(new THREE.PlaneGeometry(1000, 1000), new THREE.MeshBasicMaterial());
    plane.rotation.x = -Math.PI / 2;
    plane.visible = false;
    scene.add(plane);

    mouse2D = new THREE.Vector3(0, 10000, 0.5);

    // Lights

    var ambientLight = new THREE.AmbientLight(0x606060);
    scene.add(ambientLight);

    var directionalLight = new THREE.DirectionalLight(0xffffff);
    directionalLight.position.set(1, 0.75, 0.5).normalize();
    scene.add(directionalLight);

    renderer = new THREE.WebGLRenderer({antialias: true, preserveDrawingBuffer: true});
    renderer.setSize(jqContainer.width(), jqContainer.height());

    container.appendChild(renderer.domElement);

    stats = new Stats();
    stats.domElement.style.position = 'absolute';
    stats.domElement.style.top = '0px';
    container.appendChild(stats.domElement);

    jqContainer.mousemove(onDocumentMouseMove);
    jqContainer.mousedown(onDocumentMouseDown);
    jqContainer.mouseup(onDocumentMouseUp);

}

function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(window.innerWidth, window.innerHeight);

}

function getRealIntersector(intersects) {

    for (i = 0; i < intersects.length; i++) {

        intersector = intersects[ i ];

        if (intersector.object != rollOverMesh) {

            return intersector;

        }

    }

    return null;

}

function setVoxelPosition(intersector) {

    normalMatrix.getNormalMatrix(intersector.object.matrixWorld);

    tmpVec.copy(intersector.face.normal);
    tmpVec.applyMatrix3(normalMatrix).normalize();

    voxelPosition.addVectors(intersector.point, tmpVec);

    voxelPosition.x = Math.floor(voxelPosition.x / 50) * 50 + 25;
    voxelPosition.y = Math.floor(voxelPosition.y / 50) * 50 + 25;
    voxelPosition.z = Math.floor(voxelPosition.z / 50) * 50 + 25;

}

function onDocumentMouseMove(event) {

    event.preventDefault();
    var offset = jqContainer.offset();

    mouse2D.x = (event.clientX - offset.left) / jqContainer.width() * 2 - 1;
    mouse2D.y = -(event.clientY - offset.top) / jqContainer.height() * 2 + 1;

}

function onDocumentMouseDown(event) {

    isMouseDown = true;

    event.preventDefault();

}
function onDocumentMouseUp(event) {
    event.preventDefault();
    isMouseDown = false;
}
//

function animate() {

    requestAnimationFrame(animate);

    render();
    stats.update();

}

function render() {

    if (isMouseDown) {

        theta += 0.5;//mouse2D.x * 1.5;
        if (theta >= 360) {
            theta = 0;
        }

    }

    raycaster = projector.pickingRay(mouse2D.clone(), camera);

    var intersects = raycaster.intersectObjects(scene.children);

    if (intersects.length > 0) {

        intersector = getRealIntersector(intersects);
        if (intersector) {

            setVoxelPosition(intersector);
            rollOverMesh.position = voxelPosition;

        }

    }

    camera.position.x = 1400 * Math.sin(THREE.Math.degToRad(theta));
    camera.position.z = 1400 * Math.cos(THREE.Math.degToRad(theta));

    camera.lookAt(scene.position);

    renderer.render(scene, camera);

}