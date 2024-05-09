void DrawTextureSide(Texture2D texture, Vector2 size, Color tint)
{
    rlSetTexture(texture.id);
    SetTextureWrap(texture, RL_TEXTURE_WRAP_REPEAT);

    float halfWidth = size.x * 0.5f;
    float halfHeight = size.y * 0.5f;
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(-halfWidth, 0.0f, halfHeight); // Top-left corner

    rlTexCoord2f(2.0f, 0.0f);
    rlVertex3f(halfWidth, 0.0f, halfHeight); // Top-right corner

    rlTexCoord2f(2.0f, 2.0f);
    rlVertex3f(halfWidth, 0.0f, -halfHeight); // Bottom-right corner

    rlTexCoord2f(0.0f, 2.0f);
    rlVertex3f(-halfWidth, 0.0f, -halfHeight); // Bottom-left corner

    rlEnd();
    rlSetTexture(0);
}