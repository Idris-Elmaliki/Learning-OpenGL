# Claude Memory (copied from my main machine)

Copied on 2026-11-05 from `C:\Users\Idris Elm\.claude\projects\D--OpenGL-OpenGL-Practice\memory\`.
On another device, tell Claude: "Read memory.md and treat it as my saved memory for this project."

## Index

- [OpenGL theory and debugging only](opengl-theory-and-debugging-only.md) — explain concepts and diagnose bugs; don't write the user's OpenGL code
- [Lesson progress](lesson-progress.md) — Lesson 1 (pipeline) done 2026-10-24; next is Lesson 2 (VBOs), one subheading per reply
- [Course setup](course-setup.md) — Ben Cook's Modern OpenGL course (uses GLEW); user's project uses GLAD/GLFW/GLM

---

## opengl-theory-and-debugging-only

- **Description:** User wants Claude used only for explaining OpenGL theory and debugging — never for writing their OpenGL code
- **Type:** feedback

In this OpenGL practice repo, only help by explaining OpenGL theory/concepts and by debugging problems. Do not write or refactor the user's OpenGL code for them.

**Why:** The user is learning OpenGL and wants to write the code themselves; having Claude produce it would defeat the purpose of the practice project.

**How to apply:** Answer "why/how does this work" questions with explanation. For broken code, diagnose the cause and explain the fix in words (pointing at the relevant `file:line`) rather than editing the file. Only make edits if the user explicitly asks for them in that message.

---

## course-setup

- **Description:** The user's Udemy course and how its libraries differ from their project
- **Type:** user

The user follows "Computer Graphics with Modern OpenGL and C++" by Ben Cook (Udemy). That course loads OpenGL functions with GLEW, but the user's project uses GLAD + GLFW + GLM. The gl* calls are the same with either loader, so the theory carries over directly.

**How to apply:** write teaching snippets with GLAD/GLFW, and mention GLEW only when a loader-specific difference matters. Related: [[lesson-progress]].

---

## lesson-progress

- **Description:** Where the user is in the from-scratch OpenGL theory lessons (current lesson number and what's next)
- **Type:** project

**Lesson 1 (graphics pipeline big picture) finished on 2026-10-24.** Notes are in `notes/01-graphics-pipeline.md`. It used the factory assembly-line analogy, a CPU-vs-GPU parallelism explanation, the 9-stage diagram, an 800×600 orange-triangle worked example, clipping math, common mistakes, and the quick check.

Weak spots that came up (worth reinforcing later): clipping cuts shapes and doesn't clamp vertices; the user mixed up Vertex Specification (C++ upload) with the vertex shader's job (output position).

Next: **Lesson 2, VBOs (Vertex Buffer Objects)**, then VAOs, then shaders, etc. Teach one subheading per reply.

**Why:** the user wants to restart theory from the beginning and keep continuity across cleared chats.
**How to apply:** at the start of a session, resume from here; update this file when a lesson finishes. See [[course-setup]] and [[opengl-theory-and-debugging-only]].
