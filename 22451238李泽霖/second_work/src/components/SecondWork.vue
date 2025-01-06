<template>
  <div>{{ hitState }}</div>
  <div ref="canvasContainer" class="canvas-container"></div>
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue';
import * as mat4 from 'gl-matrix/mat4';
import * as vec3 from 'gl-matrix/vec3';

// 引用 Canvas 容器
const canvasContainer = ref<HTMLDivElement | null>(null);

const hitState = ref("");

onMounted(() => {
  if (canvasContainer.value) {
    const canvas = document.createElement('canvas');
    canvas.width = canvasContainer.value.clientWidth;
    canvas.height = canvasContainer.value.clientHeight;
    canvasContainer.value.appendChild(canvas);

    const gl = canvas.getContext('webgl');
    if (!gl) {
      console.error('WebGL not supported');
      return;
    }

    initWebGL(gl, canvas.width, canvas.height);

    // 添加鼠标点击事件
    canvas.addEventListener('click', (event) => {
      const rect = canvas.getBoundingClientRect();
      const x = ((event.clientX - rect.left) / canvas.width) * 2 - 1;
      const y = -((event.clientY - rect.top) / canvas.height) * 2 + 1;
      
      // 射线投射检测
      // 这里需要实现相应的逻辑进行击中检测
      // 例如通过计算距离等来判断是否点击到太阳或地球
      if (hitEarth(x, y)) {
        hitState.value = "Earth is hit";
        console.log("你点击了地球");
      } else if (hitSun(x, y)) {
        hitState.value = "Sun is hit";
        console.log("你点击了太阳");
      }
    });
  }
});

// 初始化 WebGL 场景
async function initWebGL(gl: WebGLRenderingContext, width: number, height: number) {
  gl.viewport(0, 0, width, height);
  gl.clearColor(0.1, 0.1, 0.1, 1.0);
  gl.enable(gl.DEPTH_TEST);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  const program = createShaderProgram(gl);
  gl.useProgram(program);

  const projectionMatrix = mat4.create();
  mat4.perspective(projectionMatrix, Math.PI / 4, width / height, 0.1, 100.0);

  const viewMatrix = mat4.create();
  const cameraPosition = vec3.fromValues(0.0, 0.0, 6.0);
  mat4.lookAt(viewMatrix, cameraPosition, [0, 0, 0], [0, 1, 0]);

  const { sphereBuffer, indicesBuffer, vertexCount } = createSphere(gl);
  const sunTexture = await initTexture(gl, 'sun_texture.png'); // 替换为太阳纹理路径
  const earthTexture = await initTexture(gl, 'earth_texture.png'); // 替换为地球纹理路径

  render(gl, program, {
    projectionMatrix,
    viewMatrix,
    sphereBuffer,
    indicesBuffer,
    vertexCount,
    sunTexture,
    earthTexture,
  });
}

// 创建 Shader 程序
function createShaderProgram(gl: WebGLRenderingContext): WebGLProgram {
  const vertexShaderSource = `
    attribute vec3 aPosition;
    attribute vec2 aTexCoord;
    attribute vec3 aNormal; // 添加法线
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    varying vec2 vTexCoord;
    varying vec3 vLighting; // 用于传递光照信息

    void main() {
      vec4 position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
      gl_Position = position;
      vTexCoord = aTexCoord;

      // 简单光照计算（假设光源在太阳位置）
      vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0)); // 光源方向
      float light = max(dot(aNormal, lightDirection), 0.0);
      vLighting = vec3(1.0, 1.0, 1.0) * light; // 光的颜色
    }
  `;

  const fragmentShaderSource = `
    precision mediump float;
    varying vec2 vTexCoord;
    varying vec3 vLighting; // 接收光照信息
    uniform sampler2D uTexture;

    void main() {
      vec4 texColor = texture2D(uTexture, vTexCoord);
      gl_FragColor = vec4(texColor.rgb * vLighting, texColor.a); // 乘以光照效果
    }
  `;

  const vertexShader = compileShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
  const fragmentShader = compileShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

  const program = gl.createProgram()!;
  gl.attachShader(program, vertexShader);
  gl.attachShader(program, fragmentShader);
  gl.linkProgram(program);

  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    console.error(`Program link error: ${gl.getProgramInfoLog(program)}`);
  }

  return program;
}

function compileShader(gl: WebGLRenderingContext, type: number, source: string): WebGLShader {
  const shader = gl.createShader(type)!;
  gl.shaderSource(shader, source);
  gl.compileShader(shader);

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    console.error(`Shader compile error: ${gl.getShaderInfoLog(shader)}`);
  }

  return shader;
}

// 创建球体
function createSphere(gl: WebGLRenderingContext) {
  const latitudeBands = 30;
  const longitudeBands = 30;
  const radius = 1;

  const positions = [];
  const texCoords = [];
  const indices = [];
  const normals = []; // 添加法线

  for (let lat = 0; lat <= latitudeBands; ++lat) {
    const theta = (lat * Math.PI) / latitudeBands;
    const sinTheta = Math.sin(theta);
    const cosTheta = Math.cos(theta);

    for (let lon = 0; lon <= longitudeBands; ++lon) {
      const phi = (lon * 2 * Math.PI) / longitudeBands;
      const sinPhi = Math.sin(phi);
      const cosPhi = Math.cos(phi);

      const x = cosPhi * sinTheta;
      const y = cosTheta;
      const z = sinPhi * sinTheta;
      const u = 1 - lon / longitudeBands;
      const v = 1 - lat / latitudeBands;

      positions.push(x * radius, y * radius, z * radius);
      texCoords.push(u, v);
      normals.push(x, y, z); // 添加法线

      if (lat < latitudeBands && lon < longitudeBands) {
        const first = lat * (longitudeBands + 1) + lon;
        const second = first + longitudeBands + 1;
        indices.push(first, second, first + 1);
        indices.push(second, second + 1, first + 1);
      }
    }
  }

  const positionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

  const texCoordBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, texCoordBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(texCoords), gl.STATIC_DRAW);

  const normalBuffer = gl.createBuffer(); // 用于法线
  gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(normals), gl.STATIC_DRAW);

  const indexBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);

  return {
    sphereBuffer: { positionBuffer, texCoordBuffer, normalBuffer },
    indicesBuffer: indexBuffer,
    vertexCount: indices.length,
  };
}

// 初始化纹理
function initTexture(gl: WebGLRenderingContext, imagePath: string): Promise<WebGLTexture> {
  return new Promise((resolve, reject) => {
    const texture = gl.createTexture()!;
    const image = new Image();
    image.src = imagePath;

    image.onload = () => {
      gl.bindTexture(gl.TEXTURE_2D, texture);
      gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
      gl.generateMipmap(gl.TEXTURE_2D);
      resolve(texture);
    };

    image.onerror = () => {
      console.error(`Failed to load texture: ${imagePath}`);
      reject(new Error(`Failed to load texture: ${imagePath}`));
    };
  });
}

// 渲染场景
function render(
  gl: WebGLRenderingContext,
  program: WebGLProgram,
  {
    projectionMatrix,
    viewMatrix,
    sphereBuffer,
    indicesBuffer,
    vertexCount,
    sunTexture,
    earthTexture,
  }: any
) {
  const modelMatrix = mat4.create();

  // 渲染太阳
  mat4.scale(modelMatrix, mat4.create(), [1.0, 1.0, 1.0]); // 修改太阳大小
  drawSphere(gl, program, modelMatrix, projectionMatrix, viewMatrix, sphereBuffer, indicesBuffer, vertexCount, sunTexture);

  // 渲染地球
  mat4.scale(modelMatrix, mat4.create(), [0.5, 0.5, 0.5]); // 修改地球大小
  mat4.translate(modelMatrix, modelMatrix, [3.0, 0.0, 0.0]); // 移动地球
  drawSphere(gl, program, modelMatrix, projectionMatrix, viewMatrix, sphereBuffer, indicesBuffer, vertexCount, earthTexture);
}

// 绘制球体
function drawSphere(
  gl: WebGLRenderingContext,
  program: WebGLProgram,
  modelMatrix: mat4,
  projectionMatrix: mat4,
  viewMatrix: mat4,
  sphereBuffer: any,
  indicesBuffer: WebGLBuffer,
  vertexCount: number,
  texture: WebGLTexture
) {
  gl.uniformMatrix4fv(gl.getUniformLocation(program, 'uModelMatrix'), false, modelMatrix);
  gl.uniformMatrix4fv(gl.getUniformLocation(program, 'uViewMatrix'), false, viewMatrix);
  gl.uniformMatrix4fv(gl.getUniformLocation(program, 'uProjectionMatrix'), false, projectionMatrix);

  gl.bindBuffer(gl.ARRAY_BUFFER, sphereBuffer.positionBuffer);
  const positionLocation = gl.getAttribLocation(program, 'aPosition');
  gl.vertexAttribPointer(positionLocation, 3, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(positionLocation);

  gl.bindBuffer(gl.ARRAY_BUFFER, sphereBuffer.texCoordBuffer);
  const texCoordLocation = gl.getAttribLocation(program, 'aTexCoord');
  gl.vertexAttribPointer(texCoordLocation, 2, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(texCoordLocation);

  gl.bindBuffer(gl.ARRAY_BUFFER, sphereBuffer.normalBuffer); // 法线
  const normalLocation = gl.getAttribLocation(program, 'aNormal');
  gl.vertexAttribPointer(normalLocation, 3, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(normalLocation);

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indicesBuffer);
  gl.bindTexture(gl.TEXTURE_2D, texture);

  gl.drawElements(gl.TRIANGLES, vertexCount, gl.UNSIGNED_SHORT, 0);
}

function hitSphere(x: number, y: number, spherePosition: vec3, sphereRadius: number): boolean {
  const rayOrigin = vec3.fromValues(0, 0, 6); // 摄像机位置
  const rayDirection = vec3.fromValues(x, y, -1); // 鼠标点击点的方向
  vec3.normalize(rayDirection, rayDirection); // 将方向向量归一化

  const L = vec3.create();
  vec3.sub(L, spherePosition, rayOrigin); // 计算球心到射线原点的向量

  const tca = vec3.dot(L, rayDirection); // 射线在球心的投影
  const d2 = vec3.dot(L, L) - tca * tca; // 从球心到射线的垂直距离平方

  if (d2 > sphereRadius * sphereRadius) return false; // 远离球的情况

  const thc = Math.sqrt(sphereRadius * sphereRadius - d2); // 交点到投影中心的距离
  const t0 = tca - thc; // 第一个交点
  const t1 = tca + thc; // 第二个交点

  return (t0 >= 0 || t1 >= 0); // 如果交点在射线的正方向上，则返回true
}

function hitEarth(x: number, y: number): boolean {
  const earthPosition = vec3.fromValues(3.0, 0.0, 0.0); // 地球的位置
  const earthRadius = 0.5; // 地球的半径
  return hitSphere(x, y, earthPosition, earthRadius);
}

function hitSun(x: number, y: number): boolean {
  const sunPosition = vec3.fromValues(0.0, 0.0, 0.0); // 太阳的位置
  const sunRadius = 1.0; // 太阳的半径
  return hitSphere(x, y, sunPosition, sunRadius);
}


</script>

<style>
.canvas-container {
  width: 750px;
  height: 750px;
}
</style>
