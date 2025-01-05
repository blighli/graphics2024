<template>
  <div>
    <h1>第三次作业</h1>
    <!-- 添加视点浏览方式说明 -->
    <p>支持多种视点浏览方式：</p>
    <ul>
      <li>以模型为中心的平移旋转和缩放</li>
      <li>以视点为中心的场景漫游</li>
    </ul>
    <!-- 添加场景漫游操作说明 -->
    <p>场景漫游操作：</p>
    <ul>
      <li>W / 上箭头：向前移动</li>
      <li>S / 下箭头：向后移动</li>
      <li>A / 左箭头：向左移动</li>
      <li>D / 右箭头：向右移动</li>
      <li>空格：向上移动</li>
      <li>Ctrl：向下移动</li>
    </ul>
    <!-- 添加文件上传按钮 -->
    <input type="file" @change="handleFileUpload" accept=".obj" />
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import * as THREE from 'three'
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader'
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls'

const scene = new THREE.Scene()
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000)
const renderer = new THREE.WebGLRenderer()
const scaleFactor = 0.5
renderer.setSize(window.innerWidth * scaleFactor, window.innerHeight * scaleFactor)
camera.aspect = (window.innerWidth * scaleFactor) / (window.innerHeight * scaleFactor)
camera.updateProjectionMatrix()

document.body.appendChild(renderer.domElement)

// 添加光源
const ambientLight = new THREE.AmbientLight(0x404040) // 环境光
scene.add(ambientLight)

const pointLight1 = new THREE.PointLight(0xffffff, 1, 100)
pointLight1.position.set(50, 50, 50)
scene.add(pointLight1)

const pointLight2 = new THREE.PointLight(0xffffff, 1, 100)
pointLight2.position.set(-50, -50, -50)
scene.add(pointLight2)

// 加载OBJ模型
const loader = new OBJLoader()


// 添加文件上传事件处理函数
const handleFileUpload = (event: Event) => {
  const input = event.target as HTMLInputElement
  const file = input.files?.[0]
  if (file) {
    const reader = new FileReader()
    reader.onload = (e) => {
      const content = e.target?.result as string
      const obj = loader.parse(content)
      scene.add(obj)
    }
    reader.readAsText(file)
  }
}

camera.position.z = 5

// 添加轨道控制器
const controls = new OrbitControls(camera, renderer.domElement)

// 添加键盘事件监听器以支持场景漫游
const keys = {
  forward: false,
  backward: false,
  left: false,
  right: false,
  up: false,
  down: false
}

document.addEventListener('keydown', (event) => {
  switch (event.key) {
    case 'ArrowUp':
    case 'w':
      keys.forward = true
      break
    case 'ArrowDown':
    case 's':
      keys.backward = true
      break
    case 'ArrowLeft':
    case 'a':
      keys.left = true
      break
    case 'ArrowRight':
    case 'd':
      keys.right = true
      break
    case ' ':
      keys.up = true
      break
    case 'Control':
      keys.down = true
      break
  }
})

document.addEventListener('keyup', (event) => {
  switch (event.key) {
    case 'ArrowUp':
    case 'w':
      keys.forward = false
      break
    case 'ArrowDown':
    case 's':
      keys.backward = false
      break
    case 'ArrowLeft':
    case 'a':
      keys.left = false
      break
    case 'ArrowRight':
    case 'd':
      keys.right = false
      break
    case ' ':
      keys.up = false
      break
    case 'Control':
      keys.down = false
      break
  }
})

const moveSpeed = 0.1

const animate = function () {
  requestAnimationFrame(animate)
  controls.update()

  // 场景漫游控制
  if (keys.forward) camera.position.z -= moveSpeed
  if (keys.backward) camera.position.z += moveSpeed
  if (keys.left) camera.position.x -= moveSpeed
  if (keys.right) camera.position.x += moveSpeed
  if (keys.up) camera.position.y += moveSpeed
  if (keys.down) camera.position.y -= moveSpeed

  renderer.render(scene, camera)
}

animate()

onMounted(() => {

})

</script>

<style scoped>

</style>