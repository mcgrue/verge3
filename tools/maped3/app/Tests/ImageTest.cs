using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

using winmaped2;

namespace winmaped2.Tests {
    [TestFixture]
    public class ImageTest : RenderFixture {
        int[] pixels;
        [SetUp]
        public void SetUp() {
            pixels = CreatePixels(16, 16, GREEN);
        }

        [Test]
        public unsafe void CanDrawOnPr2Image() {
            Image img = new Image(16, 16, pixels);

            Render.Image dest = Render.Image.create(16, 16);
            fixed (int* tiledata = img.Pixels)
                Render.renderTile32(dest, 0, 0, tiledata, true);

            int[] resultPixels = dest.getArray();
            Assert.AreEqual(pixels, resultPixels);

            dest.Dispose();
        }

        [Test]
        public void UpdatePixels() {
            Image img = new Image(16, 16, pixels);
            int[] blackPixels = CreatePixels(16, 16, BLACK);

            img.UpdatePixels(blackPixels);

            Assert.AreEqual(img.Pixels, blackPixels);

            int[] whitePixels = CreatePixels(32, 32, WHITE);
            try {
                img.UpdatePixels(whitePixels);
                Assert.Fail("Expected img.UpdatePixels(whitePixels) to fail");
            } catch (AssertionException) {
                throw;
            } catch (Exception) {
            }
        }

        [Test]
        public void Clone() {
            Image img = new Image(16, 16, CreatePixels(16, 16, BLACK));
            Image clone = img.Clone();

            Assert.AreNotSame(img, clone);
            Assert.AreNotSame(img.Pixels, clone.Pixels);
            Assert.AreEqual(img.Width, clone.Width);
            Assert.AreEqual(img.Height, clone.Height);
            Assert.AreEqual(img.Pixels, clone.Pixels);
        }

        [Test]
        public void SetPixel() {
            Image img = new Image(16, 16, CreatePixels(16, 16, BLACK));
            int x = 1;
            int y = 1;
            img.SetPixel(x, y, WHITE);

            Assert.AreEqual(WHITE, img.Pixels[y * 16 + x]);
        }

        [Test]
        public void SetPixelChecksBounds() {
            Image img = new Image(16, 16, CreatePixels(16, 16, BLACK));

            try {
                img.SetPixel(17, 0, WHITE);
                Assert.Fail();
            } catch (AssertionException) {
                throw;
            } catch (Exception) {
            }

            try {
                img.SetPixel(10, -1, WHITE);
                Assert.Fail();
            } catch (AssertionException) {
                throw;
            } catch (Exception) {
            }
        }
    }
}
