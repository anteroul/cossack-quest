#!/usr/bin/env python3
"""Generates the enemy sprite sheets and sound effects in Cossack-Quest/assets.

Run from the repo root inside `nix develop`:  python3 tools/generate_assets.py

Enemy sheets use 64x64 frames, one animation per row, frames left to right. The row order and
frame counts must match the SpriteAnimation definitions in Cossack-Quest/Enemy.cpp:
    row 0 walk (4)   row 1 attack (4)   row 2 hit (2)   row 3 death (6)   row 4 block (2, skeleton only)
The fireball sheet uses 32x32 frames: row 0 flight (4), row 1 explosion (4).
"""
import math
import random
import struct
import wave
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter

ASSETS = Path(__file__).resolve().parent.parent / "Cossack-Quest" / "assets"
FRAME = 64
GROUND = 62
COLUMNS = 6
OUTLINE = (14, 10, 10, 255)


def blank(size=FRAME):
    return Image.new("RGBA", (size, size), (0, 0, 0, 0))


def outlined(img):
    """Adds a one pixel dark outline around everything drawn, pixel-art style."""
    grown = img.getchannel("A").filter(ImageFilter.MaxFilter(3))
    out = Image.new("RGBA", img.size, OUTLINE)
    out.putalpha(grown)
    out.alpha_composite(img)
    return out


def limb(d, a, b, width, color):
    d.line([a, b], fill=color, width=width)
    r = width / 2
    for x, y in (a, b):
        d.ellipse([x - r, y - r, x + r - 1, y + r - 1], fill=color)


def circle(d, center, r, color):
    x, y = center
    d.ellipse([x - r, y - r, x + r - 1, y + r - 1], fill=color)


def fall(img, angle):
    """Tips a standing figure over around its feet, keeping it inside the frame."""
    t = math.sin(math.radians(angle))
    return img.rotate(angle, resample=Image.NEAREST, center=(32, GROUND), translate=(t * 20, -t * 14))


def blood_pool(img, width):
    pool = blank()
    d = ImageDraw.Draw(pool)
    d.ellipse([32 - width, GROUND - 5, 32 + width, GROUND + 1], fill=(88, 6, 6, 255))
    d.ellipse([32 - width + 4, GROUND - 4, 32 + width - 8, GROUND - 1], fill=(120, 12, 10, 255))
    pool.alpha_composite(img)
    return pool


def sheet(rows):
    """rows: list of frame lists. Returns the assembled sprite sheet."""
    size = rows[0][0].width
    out = Image.new("RGBA", (COLUMNS * size, len(rows) * size), (0, 0, 0, 0))
    for r, frames in enumerate(rows):
        for c, frame in enumerate(frames):
            out.alpha_composite(frame, (c * size, r * size))
    return out


# --- Wizard: a hooded, brown-robed cultist with glowing eyes who throws fireballs --------------------

ROBE = (96, 62, 38)
ROBE_DARK = (60, 38, 23)
ROBE_LIGHT = (132, 90, 55)
HOOD_SHADOW = (8, 5, 5)
EYE = (255, 150, 40)
SKIN = (150, 138, 112)
SHOE = (34, 22, 15)
ROPE = (160, 128, 74)
FIRE = [(150, 30, 8), (230, 90, 20), (255, 170, 50), (255, 240, 170)]


def fireball_in_hand(d, center, r):
    if r <= 0:
        return
    for i, color in enumerate(FIRE):
        circle(d, center, max(1, r * (1 - i * 0.24)), color)


def draw_wizard(bob=0, lean=0, sway=0, step=0, hand_r=(21, 44), hand_l=(43, 44), fire=0, eyes=True):
    img = blank()
    d = ImageDraw.Draw(img)
    top = 24 + bob  # Shoulder line
    cx = 32 + lean

    # Feet peeking out under the robe
    for side, lift in ((-1, max(0, step)), (1, max(0, -step))):
        fx = 32 + side * 6
        d.ellipse([fx - 3, GROUND - 4 - lift, fx + 3, GROUND - lift], fill=SHOE)

    # Robe with a ragged hem, shaded on the right
    hem = []
    for i, x in enumerate(range(45, 18, -3)):
        hem.append((x + sway, GROUND - 2 - (2 if i % 2 else 0)))
    d.polygon([(cx - 8, top), (cx + 8, top)] + hem, fill=ROBE)
    d.polygon([(cx + 2, top + 2), (cx + 8, top), (45 + sway, GROUND - 2), (34 + sway, GROUND - 3)], fill=ROBE_DARK)
    d.line([(cx - 5, top + 4), (24 + sway, GROUND - 5)], fill=ROBE_LIGHT, width=2)
    d.line([(cx - 1, top + 16), (30 + sway, GROUND - 4)], fill=ROBE_DARK, width=1)
    d.line([(cx + 4, top + 18), (39 + sway, GROUND - 4)], fill=(44, 28, 17), width=1)

    # Rope belt with a dangling end
    d.line([(cx - 8, top + 13), (cx + 8, top + 13)], fill=ROPE, width=2)
    d.line([(cx + 3, top + 14), (cx + 4 + sway, top + 23)], fill=ROPE, width=1)

    # Sleeves and hands
    for shoulder, hand, color in (((cx - 7, top + 3), hand_r, ROBE), ((cx + 7, top + 3), hand_l, ROBE_DARK)):
        elbow = ((shoulder[0] + hand[0]) / 2 + (-3 if hand[0] < cx else 3), (shoulder[1] + hand[1]) / 2 + 2)
        limb(d, shoulder, elbow, 6, color)
        limb(d, elbow, hand, 5, color)
        circle(d, hand, 3, SKIN)

    # Pointed hood with the face lost in shadow, only the eyes glowing
    d.polygon([(cx - 7, top - 13), (cx + 2, top - 24), (cx + 7, top - 12)], fill=ROBE)
    d.ellipse([cx - 9, top - 19, cx + 9, top + 3], fill=ROBE)
    d.ellipse([cx + 1, top - 18, cx + 9, top + 3], fill=ROBE_DARK)
    d.ellipse([cx - 5, top - 14, cx + 5, top + 1], fill=HOOD_SHADOW)
    for ex in (cx - 3, cx + 2):
        d.rectangle([ex, top - 8, ex + 1, top - 7], fill=EYE if eyes else (40, 16, 8))

    fireball_in_hand(d, (hand_r[0], hand_r[1] - 3), fire)
    return img


def wizard_sheet():
    walk = [draw_wizard(step=s, bob=b, sway=w, hand_r=(21, 44 + s), hand_l=(43, 44 - s))
            for s, b, w in ((2, 0, 1), (0, -1, 0), (-2, 0, -1), (0, -1, 0))]
    attack = [
        draw_wizard(lean=-1, hand_r=(18, 22), hand_l=(43, 40), fire=3),
        draw_wizard(lean=-2, sway=-1, hand_r=(19, 8), hand_l=(40, 34), fire=6),
        draw_wizard(lean=2, sway=1, hand_r=(30, 28), hand_l=(44, 42)),
        draw_wizard(lean=1, hand_r=(23, 38), hand_l=(43, 43)),
    ]
    hit_pose = dict(lean=-3, bob=1, hand_r=(15, 30), hand_l=(49, 30))
    hit = [draw_wizard(**hit_pose), draw_wizard(lean=-2, hand_r=(18, 36), hand_l=(46, 36))]
    standing = draw_wizard(**hit_pose)
    dead = draw_wizard(**hit_pose, eyes=False)
    death = [
        draw_wizard(**hit_pose),
        fall(draw_wizard(lean=-3, bob=2, hand_r=(14, 26), hand_l=(50, 28)), 20),
        fall(standing, 50),
        blood_pool(fall(standing, 85), 14),
        blood_pool(fall(dead, 90), 22),
        blood_pool(fall(dead, 90), 28),
    ]
    rows = [walk, attack, hit, death]
    return sheet([[outlined(f) for f in row] for row in rows])


# --- Skeleton warrior: rusty helmet, sword and a round shield it can block with ----------------------

BONE = (216, 206, 178)
BONE_DARK = (150, 138, 110)
CAVITY = (40, 30, 26)
SOCKET_GLOW = (200, 40, 24)
RUST = (122, 78, 46)
RUST_DARK = (84, 52, 30)
CLOTH = (92, 30, 30)
WOOD = (118, 76, 42)
WOOD_DARK = (82, 52, 28)
METAL = (150, 150, 160)
METAL_DARK = (92, 92, 104)
BLADE = (196, 200, 210)
BLADE_DARK = (120, 124, 136)
HILT = (96, 64, 32)


def draw_sword(d, hand, angle):
    a = math.radians(angle)
    ux, uy = math.cos(a), math.sin(a)
    px, py = -uy, ux
    tip = (hand[0] + ux * 21, hand[1] + uy * 21)
    base = (hand[0] + ux * 3, hand[1] + uy * 3)
    d.polygon([(base[0] + px * 1.6, base[1] + py * 1.6), (base[0] - px * 1.6, base[1] - py * 1.6), tip], fill=BLADE)
    d.line([base, tip], fill=BLADE_DARK, width=1)
    d.line([(base[0] + px * 4, base[1] + py * 4), (base[0] - px * 4, base[1] - py * 4)], fill=METAL_DARK, width=2)
    circle(d, (hand[0] - ux * 3, hand[1] - uy * 3), 1.5, HILT)


def draw_shield(d, center, r, spark=False):
    circle(d, center, r, METAL_DARK)
    circle(d, center, r - 2, WOOD)
    x, y = center
    for off in (-r // 2, 0, r // 2):
        d.line([(x + off, y - r + 3), (x + off, y + r - 3)], fill=WOOD_DARK, width=1)
    d.chord([x - r + 2, y - r + 2, x + r - 3, y + r - 3], 20, 160, fill=WOOD_DARK)
    circle(d, center, max(2, r // 3), METAL)
    circle(d, (x - 1, y - 1), 1, (210, 210, 220))
    if spark:
        sx, sy = x - r + 3, y - r + 4
        for dx, dy in ((0, -4), (0, 4), (-4, 0), (4, 0), (-3, -3), (3, 3), (3, -3), (-3, 3)):
            d.line([(sx, sy), (sx + dx, sy + dy)], fill=(255, 240, 160), width=1)
        circle(d, (sx, sy), 1.5, (255, 255, 230))


def draw_skeleton(bob=0, lean=0, step=0, sword_hand=(19, 40), sword_angle=105, shield=(43, 34), shield_r=9,
                  spark=False, smear=False, head=0):
    img = blank()
    d = ImageDraw.Draw(img)
    cx = 32 + lean
    hip_y = 40 + bob

    # Legs: bones with rusty boots
    for side, lift in ((-1, max(0, step)), (1, max(0, -step))):
        hip = (cx + side * 4, hip_y)
        foot = (32 + side * 6, GROUND - 3 - lift)
        knee = ((hip[0] + foot[0]) / 2 + side * 1.5, (hip[1] + foot[1]) / 2 - lift / 2)
        limb(d, hip, knee, 3, BONE)
        limb(d, knee, foot, 3, BONE_DARK if side > 0 else BONE)
        circle(d, knee, 2, BONE)
        d.rectangle([foot[0] - 3, foot[1] - 2, foot[0] + 3, foot[1] + 2], fill=RUST_DARK)

    # Pelvis and a torn loincloth
    d.ellipse([cx - 6, hip_y - 4, cx + 6, hip_y + 2], fill=BONE_DARK)
    rag = [(cx - 6, hip_y - 1), (cx + 6, hip_y - 1)]
    rag += [(cx + 6 - i * 2, hip_y + 7 - (i % 2) * 3) for i in range(7)]
    d.polygon(rag, fill=CLOTH)

    # Spine and ribcage
    top = 21 + bob
    d.line([(cx, hip_y - 3), (cx, top)], fill=BONE_DARK, width=2)
    d.ellipse([cx - 7, top + 1, cx + 7, top + 15], fill=CAVITY)
    for i in range(4):
        y = top + 3 + i * 3
        w = 7 - (i == 3) * 2
        d.line([(cx - w, y), (cx + w, y)], fill=BONE if i % 2 == 0 else BONE_DARK, width=2)
    d.line([(cx, top + 2), (cx, top + 13)], fill=BONE, width=1)
    for side in (-1, 1):
        d.ellipse([cx + side * 8 - 4, top - 2, cx + side * 8 + 4, top + 4], fill=RUST if side < 0 else RUST_DARK)

    # Skull with glowing sockets under a rusty helmet
    hx = cx + head
    sy = 6 + bob
    d.ellipse([hx - 6, sy, hx + 6, sy + 13], fill=BONE)
    d.ellipse([hx - 4, sy + 9, hx + 4, sy + 16], fill=BONE_DARK)
    for ex in (hx - 4, hx + 1):
        d.ellipse([ex, sy + 6, ex + 3, sy + 9], fill=CAVITY)
        d.point((ex + 1, sy + 7), fill=SOCKET_GLOW)
    d.point((hx, sy + 10), fill=CAVITY)
    for tx in range(hx - 3, hx + 4, 2):
        d.point((tx, sy + 13), fill=CAVITY)
    d.pieslice([hx - 7, sy - 2, hx + 7, sy + 11], 180, 360, fill=RUST)
    d.line([(hx - 7, sy + 4), (hx + 7, sy + 4)], fill=RUST_DARK, width=2)
    d.line([(hx, sy + 4), (hx, sy + 9)], fill=RUST_DARK, width=1)

    # Sword arm and sword
    shoulder = (cx - 8, top + 1)
    elbow = ((shoulder[0] + sword_hand[0]) / 2 - 2, (shoulder[1] + sword_hand[1]) / 2 + 1)
    if smear:
        for i, angle in enumerate(range(270, 130, -20)):
            a = math.radians(angle)
            d.line([(sword_hand[0] + math.cos(a) * 8, sword_hand[1] + math.sin(a) * 8),
                    (sword_hand[0] + math.cos(a) * 22, sword_hand[1] + math.sin(a) * 22)],
                   fill=(150 + i * 12, 155 + i * 12, 165 + i * 12), width=1)
    limb(d, shoulder, elbow, 3, BONE)
    limb(d, elbow, sword_hand, 3, BONE)
    draw_sword(d, sword_hand, sword_angle)
    circle(d, sword_hand, 2, BONE_DARK)

    # Shield arm and shield
    limb(d, (cx + 8, top + 1), shield, 3, BONE_DARK)
    draw_shield(d, shield, shield_r, spark)
    return img


def bone_pile(skull_x, rolled):
    img = blank()
    d = ImageDraw.Draw(img)
    rng = random.Random(7)
    draw_sword(d, (14, GROUND - 3), -8)
    for _ in range(15):
        x, y = rng.randint(10, 54), rng.randint(GROUND - 8, GROUND - 2)
        a = rng.uniform(0, math.pi)
        limb(d, (x - math.cos(a) * 5, y - math.sin(a) * 2), (x + math.cos(a) * 5, y + math.sin(a) * 2), 3,
             BONE if rng.random() < 0.6 else BONE_DARK)
    d.ellipse([33, GROUND - 7, 53, GROUND + 1], fill=METAL_DARK)
    d.ellipse([35, GROUND - 6, 51, GROUND], fill=WOOD)
    circle(d, (43, GROUND - 3), 2, METAL)
    sy = GROUND - 13 + (2 if rolled else 0)
    d.ellipse([skull_x - 6, sy, skull_x + 6, sy + 11], fill=BONE)
    for ex in (skull_x - 4, skull_x + 1):
        d.ellipse([ex, sy + 4, ex + 3, sy + 7], fill=CAVITY)
    d.pieslice([skull_x - 7, sy - 3, skull_x + 7, sy + 9], 190, 350, fill=RUST)
    return img


def skeleton_sheet():
    walk = [draw_skeleton(step=s, bob=b, sword_hand=(19, 40 - s), shield=(43, 34 + s // 2))
            for s, b in ((2, 0), (0, -1), (-2, 0), (0, -1))]
    attack = [
        draw_skeleton(lean=-1, sword_hand=(17, 16), sword_angle=-120, shield=(44, 36)),
        draw_skeleton(lean=-1, bob=-1, sword_hand=(25, 6), sword_angle=-92, shield=(44, 36)),
        draw_skeleton(lean=2, sword_hand=(25, 27), sword_angle=140, shield=(46, 37), smear=True),
        draw_skeleton(lean=1, sword_hand=(23, 42), sword_angle=118, shield=(45, 36)),
    ]
    hit_pose = dict(lean=-3, bob=1, head=-1, sword_hand=(15, 31), sword_angle=150, shield=(46, 31))
    hit = [draw_skeleton(**hit_pose), draw_skeleton(lean=-2, sword_hand=(17, 36), sword_angle=125, shield=(45, 33))]
    block = [
        draw_skeleton(sword_hand=(19, 42), sword_angle=110, shield=(34, 27), shield_r=12),
        draw_skeleton(lean=-1, bob=1, sword_hand=(18, 42), sword_angle=112, shield=(33, 27), shield_r=12, spark=True),
    ]
    standing = draw_skeleton(**hit_pose)
    death = [
        draw_skeleton(**hit_pose),
        fall(standing, 25),
        fall(standing, 60),
        bone_pile(26, False),
        bone_pile(28, True),
        bone_pile(29, True),
    ]
    rows = [walk, attack, hit, death, block]
    return sheet([[outlined(f) for f in row] for row in rows])


# --- Fireball ------------------------------------------------------------------------------------------

def fireball_sheet():
    size = 32
    rng = random.Random(3)
    flight = []
    for i in range(4):
        img = blank(size)
        d = ImageDraw.Draw(img)
        for _ in range(7):  # Flickering flame tongues
            a = rng.uniform(0, math.tau)
            r = rng.uniform(5, 9)
            circle(d, (16 + math.cos(a) * r * 0.8, 16 + math.sin(a) * r * 0.8), rng.uniform(2.5, 4), FIRE[0])
        circle(d, (16, 16), 8 + (i % 2), FIRE[1])
        circle(d, (16, 16), 6, FIRE[2])
        circle(d, (15 + i % 2, 15), 3.5, FIRE[3])
        flight.append(img)

    explosion = []
    for i, (r, smoke) in enumerate(((7, 0), (11, 0.2), (14, 0.5), (15, 1.0))):
        img = blank(size)
        d = ImageDraw.Draw(img)
        for _ in range(10):
            a = rng.uniform(0, math.tau)
            pr = rng.uniform(r * 0.4, r * 0.9)
            color = (60, 50, 46) if rng.random() < smoke else FIRE[rng.randint(0, 2)]
            circle(d, (16 + math.cos(a) * pr, 16 + math.sin(a) * pr), rng.uniform(2, 4 + i), color)
        if smoke < 1.0:
            circle(d, (16, 16), r * (1 - smoke) * 0.6, FIRE[3])
        explosion.append(img)

    out = Image.new("RGBA", (4 * size, 2 * size), (0, 0, 0, 0))
    for r, frames in enumerate((flight, explosion)):
        for c, frame in enumerate(frames):
            out.alpha_composite(frame, (c * size, r * size))
    return out


# --- Sounds --------------------------------------------------------------------------------------------

def write_wav(path, samples, rate=44100):
    with wave.open(str(path), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(b"".join(struct.pack("<h", int(max(-1, min(1, s)) * 32000)) for s in samples))


def shield_clang(rate=44100):
    """A metallic hit: inharmonic partials with fast decay plus a short noise burst."""
    rng = random.Random(11)
    partials = [(420, 1.0, 9), (1130, 0.6, 14), (1870, 0.45, 18), (2640, 0.3, 24), (3410, 0.2, 30)]
    out = []
    for n in range(int(rate * 0.6)):
        t = n / rate
        s = sum(a * math.exp(-t * decay) * math.sin(math.tau * f * t) for f, a, decay in partials)
        s += rng.uniform(-1, 1) * math.exp(-t * 60) * 0.8
        out.append(s * 0.35)
    return out


def main():
    (ASSETS / "enemies").mkdir(exist_ok=True)
    wizard_sheet().save(ASSETS / "enemies" / "wizard.png")
    skeleton_sheet().save(ASSETS / "enemies" / "skeleton.png")
    fireball_sheet().save(ASSETS / "enemies" / "fireball.png")
    write_wav(ASSETS / "sfx" / "shield_block.wav", shield_clang())
    print("Wrote assets to", ASSETS)


if __name__ == "__main__":
    main()
